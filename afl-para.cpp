#include <sys/types.h>
#include <dirent.h>
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

extern "C" {
#ifndef LOCAL_DEBUG
#include "afl-para.h"
#endif
#include "config.h"
}

using namespace std;


// Master node method
u32 waitFreeSlaves(const char* freeDir)
{
    DIR *dp;
    struct dirent *dirp;

    std::set<u32> freeIDs;
    u32  freeID;
    while (1) {
        if((dp  = opendir(freeDir)) == NULL) {
            cout << "Error(" << errno << ") opening " << freeDir << endl;
            exit(-1);
        }

        while ((dirp = readdir(dp)) != NULL) {
            if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
                continue;
            else {
                u32 id = atoi(dirp->d_name);
                if (!id) {
                    cout << "Unknown id name\n";
                    continue;
                }

                freeIDs.insert(id);
                freeID = id;
                cout << "Name is " << id << std::endl;
                char full_name[256];
                memset(full_name, 0, 256);
                sprintf(full_name, "%s/%s", freeDir, dirp->d_name);
                unlink(full_name);

                break;
            }
        }

        closedir(dp);
        if (freeIDs.size())
            break;

        sleep(WAIT_FREE);
    }

    // mark target branch ID as free
    auto it = nodeTask.find(freeID);
    u64 rbID = it->second;
    busyRBIDs.erase(rbID);

    return freeID;
}

// Master node method
u64 distributeRareSeeds(const char* masterTaskDir, const char* slaveTaskDir, u32 slaveID)
{
    DIR *dp;
    struct dirent *dirp;

    std::vector<u64> taskIDs;
    u64 taskBranchID;
    if((dp  = opendir(masterTaskDir)) == NULL) {
        cout << "Error(" << errno << ") opening " << masterTaskDir << endl;
        exit(-1);
    }

	while ((dirp = readdir(dp)) != NULL) {
		if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
			continue;
		else {
            u64 _ID = atoi(dirp->d_name);
            // If `taskBranchID` is now fuzzed by other slave, then continue.
            if (busyRBIDs.find(_ID) != busyRBIDs.end()) {
                continue;
            }
            taskIDs.push_back(_ID);
		}
	}

    closedir(dp);
    if (!taskIDs.size()) {
        cout << "There's no task in master's task directory!!!!" << "\n";
        // choose one from now-fuzzing IDs
        u32 index = rand() % busyRBIDs.size();
        std::set<u64>::const_iterator it(busyRBIDs.begin());
        advance(it, index);
        taskBranchID = *it;
    } else {
        u32 index = rand() % (taskIDs.size());
        taskBranchID = taskIDs[index];
    }
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
u64 waitTask(const char *out_dir)
{
    DIR *dp;
    struct dirent *dirp;
    char taskDir[256];
    memset(taskDir, 0, 256);
    sprintf(taskDir, "%s/task", out_dir);

    std::set<u32> branchIDs;
    u64 branchID;
    while (1) {
        if((dp  = opendir(taskDir)) == NULL) {
            cout << "Error(" << errno << ") opening " << taskDir << endl;
            exit(-1);
        }

        while ((dirp = readdir(dp)) != NULL) {
            if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
                continue;
            else if (!strcmp(dirp->d_name, "branch-hits.bin")) {
                continue;
            }
            else {
                u32 id = atoi(dirp->d_name);
                if (!id) {
                    cout << "Unknown id name\n";
                    continue;
                }

                branchIDs.insert(id);
                branchID = id;
                cout << "Name is " << id<<", branch id is "<< branchID << std::endl;

                char full_name[256];
                memset(full_name, 0, 256);
                sprintf(full_name, "%s/%s", taskDir, dirp->d_name);
                unlink(full_name);
                break;
            }
        }

        closedir(dp);
        if (branchIDs.size())
            break;

        sleep(WAIT_TASK);
    }

    return branchID;

}

// Master & slave node method
void handoverResults(u64* rareMap, const char* out_dir)
{
    char fname[256];
    FILE* fd;

    memset(fname, 0, 256);
    sprintf(fname, "%s/branch-hits.bin", out_dir);
    fd = fopen(fname, "wb");

    if (!fd) {
        cout << "Unable to open " << fname << "\n";
        exit(-1);
    }

    fwrite(rareMap, sizeof(u64), MAP_SIZE, fd);
    fclose(fd);
}

// Master node method
u8 collectResults(u64* hit_bits, const char* out_dir, u8* slaveID)
{
    // 1st: read sizeof(u64)*MAP_SIZE into buffer
    char binfile[256];
    memset(binfile, 0, 256);
    sprintf(binfile, "%s/%s/branch-hits.bin", out_dir, slaveID); //FIXME:
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
    memset((void*)slaveData, 1, sizeof(u64)*MAP_SIZE);

    fread(slaveData, sizeof(u64), MAP_SIZE, fbin);
    fclose(fbin);

    // 2nd: merge together
    u32 i = 0;
    for (; i < MAP_SIZE; i++) {
        *(hit_bits+i) += *(slaveData+i);
    }

    free(slaveData);
    return 1;
}

// Master node method
u8 calculateRarity(u64* bit_hits, const char* masterTaskDir)
{
    DIR *dp;
    struct dirent *dirp;

    if((dp  = opendir(masterTaskDir)) == NULL) {
        cout << "Error(" << errno << ") opening " << masterTaskDir << endl;
        exit(-1);
    }

    while ((dirp = readdir(dp)) != NULL) {
        if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
            continue;
        else {
            char full_name[256];
            memset(full_name, 0, 256);
            sprintf(full_name, "%s/%s", masterTaskDir, dirp->d_name);
            unlink(full_name);
        }
    }
 
    //TODO: Use real get_lowest_hit_branch_ids.
    u8 i = 10;
    char task[256];
    while (i--) {
        u32 id = rand() % MAP_SIZE;
        memset(task, 0, 256);
        sprintf(task, "%s/%d", masterTaskDir, id);
        ofstream task_file (task, fstream::trunc);
        task_file.close();
    }
    return 1;
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
u8 needRegularAFL()
{

    return 0;
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
