#ifndef AFL_PARA_H_
#define AFL_PARA_H_

#include "types.h"

#ifdef _cplusplus
extern "C" {
#endif

/*
 * Distribute initial seeds to each work node.
 * Arg1 (char *) : input directory of master node
 * Arg2 (char *) : input directory of slave node
 * Arg3 (u32)    : total number of work nodes
 * Ret  (u8)     : return 1 if succeed.
 */
u8 distributeInitSeeds(char* masterInputDir=NULL, 
                        char* slaveInputDir=NULL, 
                        u32 numWorkNodes);

/*
 * Wait for finish signal from work node.
 * Arg1 (const char*): free directory
 * Ret  (u32)        : free ID.
 */
u32 waitFreeSlaves(const char* freeDir);

/*
 * Collect bit_hits results from work node.
 * Arg1 (u64*)   : bit_hits array
 * Arg2 (u32)    : slave ID
 * Ret  (u8)     : return 1 if succeed.
 */
u8 collectResults(u64* bit_hits, u32 slaveID);

/*
 * Calculate the rarity values and store all branch IDs 
 * in master task direcotry.
 * Arg1 (u64*)        : bit_hits array
 * Arg2 (const char*) : master task directory
 * Ret  (u8)          : return 1 if succeed.
 */
u8 calculateRarity(u64* bit_hits, const char* masterTaskDir);

/*
 * Distribute rare seeds to each work node.
 * Arg1 (const char*) : task directory of master node
 * Arg2 (const char*) : task directory of slave node
 * Ret  (void)    :None.
 */
void distributeRareSeeds(const char* masterTaskDir, const char* slaveTaskDir);

/*
 * Wait for new task seeds.
 * Arg1 (const char*)  : out_dir
 * Ret  (u64)  : branch ID
 */
u64 waitTask(u32 ID);

/*
 * Hand over fuzzing results in one cycle.
 * Arg1 (u64*)        : bit_hits array
 * Arg2 (const char*) : out_dir
 * Ret  (void)        : None
 */
void handoverResults(u64* rareMap, const char* out_dir);

#ifdef _cplusplus
}
#endif

#endif // AFL_PARA_H_
