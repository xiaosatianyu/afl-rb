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
//u8 distributeInitSeeds(char* masterInputDir=NULL,
//                        char* slaveInputDir=NULL,
//                        u32 numWorkNodes);

/*
 * Wait for finish signal from work node.
 * Arg1 (const char*): free directory
 * Ret  (s32)        : free ID.
 */
s32 waitFreeSlaves(const char* freeDir);

/*
 * Collect bit_hits results from work node.
 * Arg1 (u64*)   : bit_hits array
 * Arg2 (u8*)    : slave ID in char array
 * Arg3 (u32*)   : round_new_branches
 * Ret  (u8)     : return 1 if succeed.
 */
u8 collectResults(u64* hit_bits, const char* out_dir, u8* slaveID, u32* round_new_branches);

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
 * Arg3 (u32)         : slave node ID
 * Ret  (u64)         : task branch ID.
 */
u64 distributeRareSeeds(const char* masterTaskDir, const char* slaveTaskDir, u32 slaveID);

/*
 * Wait for new task seeds.
 * Arg1 (const char*)  : out_dir
 * Ret  (u64)  : branch ID
 */
u64 waitTask(const char *out_dir);

/*
 * Hand over fuzzing results in one cycle.
 * Arg1 (u64*)        : bit_hits array
 * Arg2 (const char*) : out_dir
 * Ret  (void)        : None
 */
void handoverResults(u64* rareMap, const char* out_dir);

/*
 * Hand over total executions in one cycle.
 * Arg1 (u64)         : total_execs in this cycle
 * Arg2 (const char*) : out_dir
 * Ret  (void)        : None
 */
void handoverCycleTotalExecs(u64 cycleExecs, const char* out_dir);

/*
 * Notify master node I'm free.
 * Arg1 (const char*) : free directory
 * Arg2 (u32)         : my ID
 * Ret  (void)        : None
 */
void notifyMaster4Free(const char* freeDir, u32 slaveID);

/*
 * Check whether to use vanilla AFL.
 * Arg (const char*) : out_dir of slave
 * Ret (u8)          : Return 1 if need to.
 */
u8 needRegularAFL(const char * out_dir);

/*
 * Notify slave to run in vanilla AFL mode.
 * Arg1 (const char*)  : out_dir of master
 * Arg2 (u8*)          : slave ID in char array
 * Ret  (void)         : None
 */
void notifySlaveVanillaAFL(const char* out_dir, u8* slaveID);

/*
 * Normalize hit bits.
 * Arg1 (u64*)        : slave data
 * Arg2 (u64*)        : hit_bits
 * Arg3 (const char*) : out_dir
 * Arg4 (u8*)         : slave ID in char array
 * Ret  (void)        : None
 */
void normalizeHitBits(u64* slaveData, u64* hit_bits, const char* out_dir, u8* slaveID);

#ifdef _cplusplus
}
#endif

#endif // AFL_PARA_H_
