#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY_ 0x7777
#define SHM_TYPE_ char
#define SHM_MAX_SIZE_ 2048


int main() {

    std::cout << "pid of current process: " << getpid() << std::endl;

    int shmid;

    if ((shmid = shmget((key_t)SHM_KEY_, SHM_MAX_SIZE_, 0640|IPC_CREAT)) == -1) {
        std::cout << "shmget(" << SHM_KEY_ << ") failed." << std::endl;
        return -1;
    }

    SHM_TYPE_ *pshm = (SHM_TYPE_ *)shmat(shmid, NULL, 0);

    std:: cout << "content in shared mem before modified: " << pshm << std::endl;

    snprintf(pshm, SHM_MAX_SIZE_, "HELLO, WORLD, %d\n", getpid());

    std:: cout << "content in shared mem after modified: " << pshm << std::endl;

    shmdt(pshm);

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        std::cout << "shmctl(" << SHM_KEY_ << ") failed." << std::endl;
    }

}
