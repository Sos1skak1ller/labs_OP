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
    time_t parent_time, child_time;

    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        int fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        child_time = time(NULL);
        printf("Child PID: %d, Time: %s", getpid(), ctime(&child_time));
        read(fd, buffer, sizeof(buffer));
        printf("%s", buffer);
        close(fd);
    } else { // Родительский процесс
        int fd = open(FIFO_NAME, O_WRONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        sleep(5); 
        parent_time = time(NULL);
        char message[256];
        snprintf(message, sizeof(message), "Parent PID: %d, Time: %s", getpid(), ctime(&parent_time));
        write(fd, message, strlen(message) + 1);
        close(fd);
    }

    if (pid > 0) { 
        unlink(FIFO_NAME);
    }
    return 0;
}