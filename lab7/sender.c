#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define SHM_SIZE 128
#define LOCK_FILE "/tmp/sender.lock"
#define SHM_KEY_PATH "/tmp/mem_key"

int main() {
    int fd = open(SHM_KEY_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open SHM_KEY_PATH");
        exit(1);
    }
    close(fd);

    int lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("open LOCK_FILE");
        exit(1);
    }

    if (lockf(lock_fd, F_TLOCK, 0) == -1) {
        printf("Sender already running. Exiting...\n");
        exit(0);
    }

    key_t key = ftok(SHM_KEY_PATH, 'a');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    char *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    pid_t pid = getpid();
    while (1) {
        time_t current_time = time(NULL);
        snprintf(shmaddr, SHM_SIZE, "Time: %sPID: %d", ctime(&current_time), pid);
        printf("Written to shared memory: Time: %sPID: %d\n", ctime(&current_time), pid);
        sleep(1);
    }

    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }
    close(lock_fd);
    unlink(LOCK_FILE);

    return 0;
}