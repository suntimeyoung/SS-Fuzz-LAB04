#pragma once

#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>

#define LOG_FILE "/tmp/.fuzzlab/fuzz_log.txt"
#define TEST_PATH "/tmp/.fuzzlab/bin/test"
#define TEST_DIR "/tmp/.fuzzlab/testcases/"
#define FIFO_DATA "/tmp/.fuzzlab/fuzz_fifo_data"
#define FIFO_INST "/tmp/.fuzzlab/fuzz_fifo_inst"
#define PIPE_BUF_SIZE 1024
#define TEST_INSTANCE_NUM 1
#define CONTINUE_INST 0
#define EXIT_INST 1


void CreateNamedPipe(const char *fifo_path);
void MakeFileFromSeed(const char *file_name, int seed);
void SendInst(const int inst_pipe_fd, int continue_or_wait);
