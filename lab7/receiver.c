#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define SHM_SIZE 128
#define SHM_KEY_PATH "/tmp/mem_key"

int main() {
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

    int shmid = shmget(key, SHM_SIZE, 0666);
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
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        printf("Current Time: %s, PID: %d, Received: %s\n", time_str, pid, shmaddr);
        sleep(1);
    }

    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}