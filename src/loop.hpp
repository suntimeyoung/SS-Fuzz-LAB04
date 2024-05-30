#pragma once

#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctime>

#include <iostream>
#include <fstream>
#include <vector>

#define DEFAULT_TEST_PROG "test.cpp.elf"
#define TEST_PROG_DIR "/tmp/.fuzzlab/bin/"
#define TEST_DIR "/tmp/.fuzzlab/testcases/"
#define FIFO_DATA "/tmp/.fuzzlab/fuzz_fifo_data"
#define FIFO_INST "/tmp/.fuzzlab/fuzz_fifo_inst"
#define PIPE_BUF_SIZE 512
#define TEST_NUM_PER_ROUND 10
#define TEST_NUM_MAX 100

#define CONTINUE_INST 0
#define WAIT_INST 1
#define EXIT_INST 2

#define FSHM_KEY 0x7777
#define FSHM_TIME_KEY 0x8888
#define SHM_COUNTER_TYPE Int32Ty
#define FSHM_TYPE unsigned int
#define FSHM_MAX_ITEM_POW2 16

struct PidTime {
    clock_t start_time;
    clock_t duration_time;
    pid_t pid;
};


std::vector<char *> GetInitTestList();
int OpenNamedPipe(const char *fifo_path, int flag);
int GetOrCreateSharedMem(key_t key, size_t size);
void ReadSharedMem(char *buf, int buf_size);
void WriteSharedMem(const char *buf, int pos, int offset);
void DeleteSharedMem(int shmid);
void SendInst(const int inst_pipe_fd, int continue_or_wait);
void ReceiveInst(const int inst_pipe_fd);
bool ProgFinish(PidTime* pid_time_ptr);
