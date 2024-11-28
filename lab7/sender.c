#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#define SHM_SIZE 1024
#define LOCK_FILE "/tmp/sender.lock"

struct shared_data {
    pid_t pid;
    char timestamp[26];
};

int main() {
    // Check if another instance is running
    int lock_fd = open(LOCK_FILE, O_CREAT | O_EXCL, 0644);
    if (lock_fd == -1) {
        printf("Another instance is already running\n");
        return 1;
    }

    // Create shared memory
    key_t key = ftok("/tmp", 'A');
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        unlink(LOCK_FILE);
        return 1;
    }

    struct shared_data *shared_mem = shmat(shmid, NULL, 0);
    if (shared_mem == (void *)-1) {
        perror("shmat failed");
        unlink(LOCK_FILE);
        return 1;
    }

    while (1) {
        time_t now = time(NULL);
        shared_mem->pid = getpid();
        strftime(shared_mem->timestamp, 26, "%Y-%m-%d %H:%M:%S", localtime(&now));
        sleep(1);
    }

    shmdt(shared_mem);
    unlink(LOCK_FILE);
    return 0;
}