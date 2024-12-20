#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define SHM_SIZE 128
#define SHM_KEY_PATH "/tmp/memsemaphorekusha_key"
#define SEM_KEY_PATH "/tmp/semaphorekusha_key"

int main() {
    int fd = open(SHM_KEY_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
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

    key_t sem_key = ftok(SEM_KEY_PATH, 'b');
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(1);
    }

    pid_t pid = getpid();
    struct sembuf sops;

    while (1) {
        sops.sem_num = 0;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if (semop(semid, &sops, 1) == -1) {
            perror("semop wait");
            exit(1);
        }

        time_t current_time = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        printf("Current Time: %s, PID: %d, Received: %s\n", time_str, pid, shmaddr);

        sops.sem_op = 1;
        if (semop(semid, &sops, 1) == -1) {
            perror("semop signal");
            exit(1);
        }

        sleep(1);
    }

    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
