#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define FIFO_NAME "my_fifo"

int main() {
    pid_t pid;
    char buffer[256];
    time_t parent_time;

    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { // Родительский процесс
        int fd = open(FIFO_NAME, O_WRONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        parent_time = time(NULL);
        char message[256];
        snprintf(message, sizeof(message), "Parent PID: %d, Time: %s", getpid(), ctime(&parent_time));
        write(fd, message, strlen(message) + 1);
        close(fd);
        sleep(5); // Ждем 5 секунд
    } else { // Дочерний процесс
        int fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        time_t child_time = time(NULL);
        read(fd, buffer, sizeof(buffer));
        printf("Child PID: %d, Current Time: %s%s", getpid(), ctime(&child_time), buffer);
        close(fd);
    }

    unlink(FIFO_NAME);
    return 0;
}
