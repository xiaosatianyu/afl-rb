#include <sys/types.h>
#include <dirent.h>
#include <stdarg.h>
#include <errno.h>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
#include <unistd.h>
#include <fstream> 
#include <iostream>

#include "types.h"

/* These variables should be only modified in master node! Should above `afl-para.h` */
std::map<u32, u64> nodeTask;   // Node ID ---- Branch ID
std::set<u64>      busyRBIDs;  // all busy branch IDs
typedef std::map<u64,u64> ID_HOT;

int cmp(const std::pair<u64, u64>& x, const std::pair<u64, u64>& y)    
{    
    return x.second < y.second;    
}  
void sortMapByValue(ID_HOT& tMap, std::vector<std::pair<u64, u64> >& tVector)    
{    
    for (auto curr = tMap.begin(); curr != tMap.end(); curr++)     
        tVector.push_back(std::make_pair(curr->first, curr->second));      
         
    std::sort(tVector.begin(), tVector.end(), cmp);    
} 

ID_HOT fuzzedIDs; //key is the ID, value is the cycle that this ID has been fuzzed

extern "C" {
#ifndef LOCAL_DEBUG
#include "afl-para.h"
#endif
#include "config.h"
}

using namespace std;

double nmHitBits[MAP_SIZE];

#define DEBUG fileonly
void fileonly (char const *fmt, ...) { 
    static FILE *f = NULL;
    if (f == NULL) {
      char fn[256];
      memset(fn, 0, 256);
      sprintf(fn, "%s/para-test", "/tmp");
      f= fopen(fn, "w");
    }
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fflush(f);
}

// Master node method
// return -1 if there is no free slave
s32 waitFreeSlaves(const char* freeDir)
{
    DIR *dp;
    struct dirent *dirp;
    u8 get_slave=0;
    u32  freeID ; // means the slave id

    if((dp  = opendir(freeDir)) == NULL) {
        cout << "Error(" << errno << ") opening " << freeDir << endl;
        cout << "can not open free dir\n";
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL) {
        if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
            continue;
        else {
            u32 id = atoi(dirp->d_name);
            freeID = id;
            DEBUG("Free slave ID is %d\n", id);
            char full_name[256];
            memset(full_name, 0, 256);
            sprintf(full_name, "%s/%s", freeDir, dirp->d_name);
            unlink(full_name);
            get_slave=1;
            break;
        }
    }
    closedir(dp);

    if (!get_slave)
        return -1;  // there is no new tasks

    // mark target branch ID as free
    auto it = nodeTask.find(freeID);
    u64 rbID = it->second;
    busyRBIDs.erase(rbID); //remove the prev branch id of this slave

    return freeID;
}

// get a id from task
  
u64 getTaskId(const char* masterTaskDir){
    
    DIR *dp;
    struct dirent *dirp;
    std::set<u64> taskIDs;  // current task IDs
    u8 newflag = 0; 
    u64 targetID=0;
    
    //1. read ids from task poll
    if((dp  = opendir(masterTaskDir)) == NULL) {
        cout << "Error(" << errno << ") opening " << masterTaskDir << endl;
        cout << "can not oepn task dir\n";
        DEBUG("can not open task dir\n");
        //return -1;
        //exit(-1);
    }
    u64 _ID;
    while ( dp!= NULL && (dirp = readdir(dp)) != NULL) {
		if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
			continue;
		else {
             _ID = atoi(dirp->d_name);
            // check if it is a new id
            if ( fuzzedIDs.find(_ID) == fuzzedIDs.end() )
            {   
                // it is a new id
                newflag=1;
                targetID=_ID;
                break;
            }
            taskIDs.insert(_ID);
		}
	}
    
    if (newflag){
        // it is a new id
        DEBUG("Finding new rare branch %d\n", targetID);
        fuzzedIDs.insert(std::make_pair(targetID,1));
    }
    else{
        // there is no new id ,return a least executed id

        // find a task acoording the hot and it must in the current tasks
        // and it must be an old id, e.g it is in the fuzzedIDs
        DEBUG("ALL tasks in master are old, try to select a ID both in current tasks and old tasks\n");
        std::vector<std::pair<u64, u64> > sortedFuzzedIDs;    
        sortMapByValue(fuzzedIDs, sortedFuzzedIDs);
        int size=sortedFuzzedIDs.size();
        u64 ID;
        u64 hot;
        while(size--){
            auto it = sortedFuzzedIDs[size-1];
            ID = it.first;
            hot = it.second;
            if ( taskIDs.find(ID) != taskIDs.end() ){
                targetID = ID;
                fuzzedIDs[targetID] += 1;
                DEBUG("Using prev rare branch %d with hot: %d\n", ID, hot);
                break;
            }
            else{
                targetID=0;
                 DEBUG("[para] why here, in getTaskId function--------------------------");
            }
        }
    }

    return targetID;
} 


// Master node method
u64 distributeRareSeeds(const char* masterTaskDir, const char* slaveTaskDir, u32 slaveID)
{

    u64 taskBranchID;
    taskBranchID = getTaskId(masterTaskDir); 
	
    string newName = string(slaveTaskDir);
    newName += "/";
    newName += std::to_string(taskBranchID);
    ofstream task (newName.c_str(), fstream::trunc);
    task.close();

    // remove task from master 
    string oldName = string(masterTaskDir);
    oldName += "/";
    oldName += std::to_string(taskBranchID);
    unlink(oldName.c_str());

    if (nodeTask.find(slaveID) == nodeTask.end()) {
        nodeTask.insert(std::make_pair(slaveID, taskBranchID));
    } else {
        nodeTask[slaveID] = taskBranchID;
    }

    // mark taskBranchID as busy
    busyRBIDs.insert(taskBranchID);

    return taskBranchID;
}

// Slave node method
u64 waitTask(const char *out_dir, u8* get_task_flag)
{
    DIR *dp;
    struct dirent *dirp;
    char taskDir[256];
    memset(taskDir, 0, 256);
    sprintf(taskDir, "%s/task", out_dir);

    u64 branchID =0;//default is 0
    u32 id;
    *get_task_flag =0;
    u8 wait_num=2;
    while (wait_num--) {
        if((dp  = opendir(taskDir)) == NULL) {
            cout << "Error(" << errno << ") opening " << taskDir << endl;
            cout << "can not open task dir in slave";
            //exit(-1);
        }

        while ( dp!= NULL && (dirp = readdir(dp)) != NULL) {
            if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
                continue;
            else if (!strcmp(dirp->d_name, "branch-hits.bin")) {
                continue;
            }
            else {
                id = atoi(dirp->d_name);
                // read a task and check, remove it no matter if it is right
                if (id<0 || id > 65535) {
                    cout << "Unknown id name\n";
                    cout << "id is not between 0,65535\n";
                    cout << "remove this stragne task\n";    
                }
                else{
                    branchID = id;
                    *get_task_flag = 1 ;
                    DEBUG("Task ID is %d\n", branchID);
                }

                //remove the task file
                char full_name[256];
                memset(full_name, 0, 256);
                sprintf(full_name, "%s/%s", taskDir, dirp->d_name);
                unlink(full_name);
                
                if ( get_task_flag)
                {
                   return branchID;
                }
            }
        }
        if (!dp) closedir(dp);

        cout << "there is no task, wait for some time\n";
        sleep(WAIT_TASK);

    }//end while
    
    cout << "do not get a task from master, save itself\n";
    return 0; // 这里0不是指branch0,实在要返回一个,所以没办法,还要根据get_task_flag一起判定
}

// Master & slave node method
//void handoverResults(u64* rareMap, const char* out_dir)
//{
//    char fname[256];
//    FILE* fd;
//
//    memset(fname, 0, 256);
//    sprintf(fname, "%s/branch-hits.bin", out_dir);
//    fd = fopen(fname, "wb");
//
//    if (!fd) {
//        cout << "Unable to open " << fname << "\n";
//        exit(-1);
//    }
//
//    fwrite(rareMap, sizeof(u64), MAP_SIZE, fd);
//    fclose(fd);
//}

// Slave node method
// TODO: Use macro to avoid duplication.
//void handoverCycleTotalExecs(u64 cycleExecs, const char* out_dir)
//{
//    char fname[256];
//    FILE* fd;
//
//    memset(fname, 0, 256);
//    sprintf(fname, "%s/cycle_execs", out_dir);
//    fd = fopen(fname, "wb");
//
//    if (!fd) {
//        cout << "Unable to open " << fname << "\n";
//        cout << "can not open cycle_execs";
//        exit(-1);
//    }
//
//    char buff[256];
//    memset(buff, 0, 256);
//    sprintf(buff, "%llu", cycleExecs);
//    fwrite(buff, sizeof(char), 256, fd);
//    fclose(fd);
//}

//void normalizeHitBits(u64* slaveData, u64* hit_bits, const char* out_dir, u8* slaveID)
//{
//    char fname[256];
//    FILE* fd;
//
//    memset(fname, 0, 256);
//    sprintf(fname, "%s/%s/cycle_execs", out_dir, slaveID);
//    fd = fopen(fname, "r");
//    if (!fd) {
//        DEBUG("unable to open %s\n", fname);
//        exit(-1);
//    }
//
//    char buff[256];
//    memset(buff, 0, 256);
//    fread(buff, sizeof(char), 256, fd);
//    fclose(fd);
//    unlink(fname);
//
//    u64 cycle_execs = atoi(buff);
//    if (!cycle_execs) {
//        DEBUG("Slave %s runs 0 test cases in one cycle!!!!\n", slaveID);
//        exit(-1);
//    }
//    DEBUG("Slave %s runs %llu test cases in one cycle.\n", slaveID, cycle_execs);
//
//#define EXPAND_FACTOR 1e+12 //FIXME: we assume that 1e+12 is the maximum number in one cycle
//    for (u64 i=0; i < MAP_SIZE; i++) {
//        nmHitBits[i] += slaveData[i] / ((double) cycle_execs);
//        hit_bits[i]  =  (u64)(nmHitBits[i] * EXPAND_FACTOR);
//    }
//#undef EXPAND_FACOTR
//    
//    return;
//}

// Master node method
//round_new_branches is set as the number of the new branch detected in this cycle of the slave
//return 0 if the slave skip the fuzz_one function, return 1 if collect info success.
u8 collectResults(u64* hit_bits, const char* out_dir, u8* slaveID, u32* round_new_branches)
{
    // 1st: read sizeof(u64)*MAP_SIZE into buffer
    char binfile[256];
    FILE *fbin;
#if 0 // Do nothing since we use shared memory to update hit_bits.
    memset(binfile, 0, 256);
    sprintf(binfile, "%s/%s/branch-hits.bin", out_dir, slaveID);
    FILE *fbin = fopen(binfile, "rb");
    if (!fbin) {
        cout << "Cannot open file: " << binfile << "\n";
        exit(-1);
    }

    u64* slaveData = (u64*) malloc(sizeof(u64)*MAP_SIZE);
    if (!slaveData) {
        cout << "Malloc for reading slave data failed\n";
        exit(0);
    }
    memset((void*)slaveData, 0, sizeof(u64)*MAP_SIZE);

    fread(slaveData, sizeof(u64), MAP_SIZE, fbin);
    fclose(fbin);

    // normalize slave data get more accurate hit_bits
    normalizeHitBits(slaveData, hit_bits, out_dir, slaveID);
    free(slaveData);
#endif 
    // 3rd: collect new branches
    memset(binfile, 0, 256);
    sprintf(binfile, "%s/%s/newbranches", out_dir, slaveID);
    fbin = fopen(binfile, "rb");
    if (!fbin) {
        //cout << "Cannot open file: " << binfile << "\n";
        cout << slaveID << " skip a fuzz_one fucntion\n"; 
        return 1; //skip
    }

    char buff[256];
    memset(buff, 0, 256);
    fread(buff, 1, 256, fbin);

    u32 new_branches = atoi(buff);
    *round_new_branches = new_branches;

    fclose(fbin);
    unlink(binfile);

    return 0; //get 
}

// Slave node method
void notifyMaster4Free(const char* freeDir, u32 slaveID)
{
    char freeFile[256];
    memset(freeFile, 0, 256);
    sprintf(freeFile, "%s/%d", freeDir, slaveID);
    ofstream free_file (freeFile, fstream::trunc);
    free_file.close();
}

// Slave node method
// read from current dir
u8 needRegularAFL(const char* out_dir)
{
    char notifyFile[256];
    memset(notifyFile, 0, 256);
    sprintf(notifyFile, "%s/vanilla.AFL", out_dir);

    if (access(notifyFile, F_OK ) != -1) {
        unlink(notifyFile);
        return 1;
    }

    return 0;
}

// Master node method
// save a file in the slave dir
void notifySlaveVanillaAFL(const char* out_dir, u8* slaveID)
{
    char binfile[256];
    memset(binfile, 0, 256);
    sprintf(binfile, "%s/../%s/vanilla.AFL", out_dir, slaveID);
    ofstream notify_file (binfile, fstream::trunc);
    notify_file.close();
}

#ifdef LOCAL_DEBUG
int main()
{
    u64* data = (u64*) malloc(sizeof(u64)*MAP_SIZE);
    if (!data) {
        cout << "hsifdsfds\n";
        exit(0);
    }
    memset((void*)data, 1, sizeof(u64)*MAP_SIZE);
    handoverResults(data, "shit/");
    return 1;
}
#endif
