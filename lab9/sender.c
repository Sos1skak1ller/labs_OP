#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>
#include <string.h>

#define SHM_SIZE 256
#define LOCKFILE "./sender.lock"
#define SHMFILE "./shmfile"
#define SEMFILE "./semfile"

void get_current_time(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info);
}

void handle_existing_instance() {
    printf("Передающий процесс уже запущен. Завершение.\n");
    exit(EXIT_FAILURE);
}

void sem_operation(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("Ошибка операции с семафором");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int lock_fd = open(LOCKFILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("Ошибка открытия lock-файла");
        exit(EXIT_FAILURE);
    }

    if (flock(lock_fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            handle_existing_instance();
        } else {
            perror("Ошибка установки блокировки");
            close(lock_fd);
            exit(EXIT_FAILURE);
        }
    }

    FILE *file = fopen(SHMFILE, "a");
    if (!file) {
        perror("Ошибка создания shmfile");
        close(lock_fd);
        exit(EXIT_FAILURE);
    }
    fclose(file);
    chmod(SHMFILE, 0666);

    key_t shm_key = ftok(SHMFILE, 65);
    key_t sem_key = ftok(SEMFILE, 66);
    if (shm_key == -1 || sem_key == -1) {
        perror("Ошибка ftok");
        close(lock_fd);
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(shm_key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("Ошибка shmget");
        close(lock_fd);
        exit(EXIT_FAILURE);
    }

    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (char *)(-1)) {
        perror("Ошибка shmat");
        close(lock_fd);
        exit(EXIT_FAILURE);
    }

    int semid = semget(sem_key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("Ошибка создания семафора");
        close(lock_fd);
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("Ошибка инициализации семафора");
        close(lock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Передающий процесс запущен. PID: %d\n", getpid());

    while (1) {
        char time_buffer[64];
        get_current_time(time_buffer, sizeof(time_buffer));

        sem_operation(semid, -1);
        snprintf(shared_memory, SHM_SIZE, "PID: %d, Время: %s", getpid(), time_buffer);
        sem_operation(semid, 1);

        sleep(2); 
    }

    shmdt(shared_memory);


    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Ошибка удаления сегмента разделяемой памяти");
    }


    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("Ошибка удаления семафора");
    }



    close(lock_fd);
    unlink(LOCKFILE);

    return 0;
}