#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#define SHM_SIZE 128
#define SHM_KEY_PATH "/tmp/mem_key"
#define LOCK_FILE "/tmp/sender.lock"

int main() {

    int lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("open LOCK_FILE");
        exit(1);
    }
    close(lock_fd);

    int fd = open(SHM_KEY_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open SHM_KEY_PATH");
        exit(1);
    }
    close(fd);

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

    while (1) {
        time_t current_time = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        snprintf(shmaddr, SHM_SIZE, "Current Time: %s", time_str);
        sleep(1);
    }

    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}