#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>

#define SHM_SIZE 128
#define SHM_KEY_PATH "/tmp/mem_key"
#define SEM_KEY_PATH "/tmp/sem_key"

int main() {
    key_t shm_key = ftok(SHM_KEY_PATH, 'a');
    if (shm_key == -1) {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(shm_key, SHM_SIZE, 0666);
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
    if (sem_key == -1) {
        perror("ftok");
        exit(1);
    }

    int semid = semget(sem_key, 1, 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    struct sembuf sem_op;

    while (1) {
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        if (semop(semid, &sem_op, 1) == -1) {
            perror("semop lock");
            exit(1);
        }

        printf("Read from shared memory: %s\n", shmaddr);

        sem_op.sem_op = 1;
        if (semop(semid, &sem_op, 1) == -1) {
            perror("semop unlock");
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