#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define SHM_SIZE 128
#define SHM_KEY_PATH "/tmp/mem_key"
#define SEM_KEY_PATH "/tmp/sem_key"


int main() {
    int fd = open(SHM_KEY_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open SHM_KEY_PATH");
        exit(1);
    }
    close(fd);

    key_t shm_key = ftok(SHM_KEY_PATH, 'a');
    if (shm_key == -1) {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
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

    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    union semun sem_union;
    sem_union.val = 1;
    if (semctl(semid, 0, SETVAL, sem_union) == -1) {
        perror("semctl");
        exit(1);
    }

    struct sembuf sem_op;

    pid_t pid = getpid();
    while (1) {
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        if (semop(semid, &sem_op, 1) == -1) {
            perror("semop lock");
            exit(1);
        }

        time_t current_time = time(NULL);
        snprintf(shmaddr, SHM_SIZE, "Time: %sPID: %d", ctime(&current_time), pid);
        printf("Written to shared memory: Time: %sPID: %d\n", ctime(&current_time), pid);

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