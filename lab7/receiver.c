#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHM_SIZE 1024

struct shared_data {
    pid_t pid;
    char timestamp[26];
};

int main() {
    // Attach to shared memory
    key_t key = ftok("/tmp", 'A');
    int shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    struct shared_data *shared_mem = shmat(shmid, NULL, 0);
    if (shared_mem == (void *)-1) {
        perror("shmat failed");
        return 1;
    }

    while (1) {
        time_t now = time(NULL);
        char current_time[26];
        strftime(current_time, 26, "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        printf("Receiver PID: %d, Time: %s\n", getpid(), current_time);
        printf("Received from PID %d: %s\n\n", 
               shared_mem->pid, shared_mem->timestamp);
        sleep(1);
    }

    shmdt(shared_mem);
    return 0;
}