#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#define SHM_SIZE 128
#define LOCK_FILE "/tmp/sendersemaphor.lock"
#define SHM_KEY_PATH "/tmp/memsemaphor_key"
#define SEM_KEY_PATH "/tmp/semaphor_key"

int lock_fd;
int shmid;
int semid;  

void cleanup() {
    close(lock_fd);
    unlink(LOCK_FILE);

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
    } else {
        printf("Shared memory removed\n");
    }

    if (semctl(semid, 0, IPC_RMID, 0) == -1) {
        perror("semctl");
    } else {
        printf("Semaphore removed\n");
    }

    printf("Lock file removed\n");
}

void handle_signal(int signum) {
    cleanup();
    exit(0);
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    int fd = open(SHM_KEY_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    close(fd);

    lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("open lock file");
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

    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
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
    semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    struct sembuf sops;

    pid_t pid = getpid();
    while (1) {
        sops.sem_num = 0;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if (semop(semid, &sops, 1) == -1) {
            perror("semop wait");
            exit(1);
        }

        time_t current_time = time(NULL);
        snprintf(shmaddr, SHM_SIZE, "Time: %sPID: %d", ctime(&current_time), pid);

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