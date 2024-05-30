#include "loop.hpp"

std::vector<char *> GetInitTestList() {
    DIR *testcase_dir;
    struct dirent *ent;
    std::vector<char *> res;

    if ((testcase_dir = opendir(TEST_DIR)) != NULL) {
        while ((ent = readdir(testcase_dir)) != NULL) {
            if (ent->d_name[0] == '.') continue;
            char *init_file = (char *)malloc(sizeof(ent->d_name));
            if (init_file) {
                snprintf(init_file, PIPE_BUF_SIZE, "%s%s", TEST_DIR, ent->d_name);
                res.push_back(init_file);
            } else {
                fprintf(stderr, "Could not malloc for size %lu\n", sizeof(ent->d_name));
                exit(EXIT_FAILURE);
            }
        }
        closedir(testcase_dir);
    } else {
        fprintf(stderr, "Could not access testcase dir: %s\n", TEST_DIR);
        exit(EXIT_FAILURE);
    }

    return res;

}

int OpenNamedPipe(const char *fifo_path, int flag) {
    if(access(fifo_path, F_OK) == -1) {
        if(mkfifo(fifo_path, 0777) != 0) {
            fprintf(stderr, "Could not create fifo %s\n", fifo_path);
            exit(EXIT_FAILURE);
        }
    }

    int pipe_fd = open(fifo_path, flag);
    if (pipe_fd == -1) {
        fprintf(stderr, "Open error on named pipe: %s\n", fifo_path);
        exit(EXIT_FAILURE);
    }

    return pipe_fd;
}

int GetOrCreateSharedMem(key_t key, size_t size) {
    int shmid;
    
    /* Create a SHM with TEST_INSTANCE_NUM rows and (1 << FSHM_MAX_ITEM_POW2) columns of type FSHM_TYPE */
    if ((shmid = shmget((key_t)key, size, 0666|IPC_CREAT)) == -1) {
        fprintf(stderr, "Could not create shared memory with key %x\n", FSHM_KEY);
        exit(EXIT_FAILURE);
    } else {
        return shmid;
    }
}

void ReadSharedMem(char *buf, int buf_size) {

}

void WriteSharedMem(const char *buf, int pos, int offset) {

}

void DeleteSharedMem(int shmid) {

}

void SendInst(const int inst_pipe_fd, const int continue_or_wait) {
    if (inst_pipe_fd != -1) {
        if (write(inst_pipe_fd, &continue_or_wait, sizeof(continue_or_wait)) == -1) {
            fprintf(stderr, "Write error on pipe %s\n", FIFO_INST);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Open error on named pipe: %s\n", FIFO_DATA);
        exit(EXIT_FAILURE);
    }
}

void ReceiveInst(const int inst_pipe_fd) {
    char inst_buf[PIPE_BUF_SIZE + 1];
    int inst;
            
    if (read(inst_pipe_fd, inst_buf, sizeof(int)) > 0) {
        inst = *((int *)inst_buf);

        switch (inst)
        {
        case CONTINUE_INST:
            break;
        
        case WAIT_INST:
            std::cout << "(pid:" << getpid() << ") Parent: get WAIT_INST from fuzz_main. Sleeping..." << std::endl;
            while (read(inst_pipe_fd, inst_buf, sizeof(int)) > 0) {
                inst = *((int *)inst_buf);
                switch (inst)
                {
                case CONTINUE_INST:
                    std::cout << "(pid:" << getpid() << ") Parent: get CONTINUE_INST from fuzz_main. Awaked." << std::endl;
                    return;

                case WAIT_INST:
                    break;
                
                case EXIT_INST:
                    std::cout << "(pid:" << getpid() << ") Parent: get EXIT_INST from fuzz_main. Exited." << std::endl;
                    exit(EXIT_SUCCESS);
                
                default:
                    fprintf(stderr, "Bad inst.");
                    exit(EXIT_FAILURE);
                }
            }
        
        case EXIT_INST:
            std::cout << "(pid:" << getpid() << ") Parent: get EXIT_INST from fuzz_main. Exited." << std::endl;
            exit(EXIT_SUCCESS);
        
        default:
            fprintf(stderr, "Bad inst.");
            exit(EXIT_FAILURE);

        }

    }
}

bool ProgFinish(PidTime* pid_time_ptr) {
    bool check = true;
    for (int i=0; i<TEST_NUM_PER_ROUND; i++) {

        if (pid_time_ptr[i].duration_time != 0) continue; // pid recorded

        int status;
        pid_t result = waitpid(pid_time_ptr[i].pid, &status, WNOHANG);

        if (result == 0) {
            // pid running
            check = false;

        } else if (result == -1) {
            pid_time_ptr[i].duration_time = clock() - pid_time_ptr[i].start_time; 
            // fprintf(stderr, "Bad child process %d.\n", pid_time_ptr[i].pid);
            // exit(EXIT_FAILURE);
        }
        //  else {
        //     // pid finished
        //     pid_time_ptr[i].duration_time = clock() - pid_time_ptr[i].start_time; 
        // }
    }
    return check;
}
