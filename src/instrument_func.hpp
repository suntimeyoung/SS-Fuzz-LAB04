#pragma once

#include <set>

#define FSHM_KEY 0x7777
#define FSHM_COUNTER_TYPE Int32Ty
#define FSHM_TYPE unsigned int
#define FSHM_MAX_ITEM_POW2 16

unsigned long long last_add = 0;
std::set<unsigned long long> branches;

extern FSHM_TYPE *__afl_area_ptr;
extern int *__afl_prev_loc;
static int prev_loc = 0;

extern void ForkServer();
int GetOrCreateSharedMem();
void ReadSharedMem(char *buf, int buf_size);
void WriteSharedMem(const char *buf, int pos, int offset);
void DeleteSharedMem(int shmid);

