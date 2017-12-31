#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <set>
#include <string>
#include <string.h>
#include <unistd.h>
#include <fstream> 
#include <iostream>

#include "types.h"
extern "C" {
#ifndef LOCAL_DEBUG
#include "afl-para.h"
#endif
#include "config.h"
}



using namespace std;

//u8 distributeInitSeeds(char* masterInputDir, char* slaveInputDir, u32 numWorkNodes)
//{
//    return 1;
//}


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

    return freeID;
}

u64 distributeRareSeeds(const char* masterTaskDir, const char* slaveTaskDir)
{
    u32 taskNum = 1; // Currently we distribute only one task each time
    DIR *dp;
    struct dirent *dirp;

    u64 taskBranchID;
    if((dp  = opendir(masterTaskDir)) == NULL) {
        cout << "Error(" << errno << ") opening " << masterTaskDir << endl;
        exit(-1);
    }

    //先清空原来的task
//	u8 * fn;
//	fn = alloc_printf("%s/task", out_dir);
//	if (delete_files(fn, NULL)) PFATAL("Unable to remove '%s'", fn);
//	if (mkdir(fn, 0700)) PFATAL("Unable to create '%s'", fn);
//	ck_free(fn);

    u32 touched = 0;
	while ((dirp = readdir(dp)) != NULL && taskNum) {
		if (!strcmp(dirp->d_name, "..") || !strcmp(dirp->d_name, "."))
			continue;
		else {
			string newName = string(slaveTaskDir);
			newName += "/";
			newName += string(dirp->d_name);
            taskBranchID = atoi(dirp->d_name);
			ofstream task (newName.c_str(), fstream::trunc);
			task.close();
			touched++;
			taskNum -= 1;
		}
	}

    closedir(dp);

    if (!touched) {
        cout << "There's no task in master's task directory!!!!" << "\n";
        exit(-1);
    }

    return taskBranchID;
}

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
                cout << "Name is " << id << std::endl;

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

void notifyMaster4Free(const char* freeDir, u32 slaveID)
{
    char freeFile[256];
    memset(freeFile, 0, 256);
    sprintf(freeFile, "%s/%d", freeDir, slaveID);
    ofstream free_file (freeFile, fstream::trunc);
    free_file.close();
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
