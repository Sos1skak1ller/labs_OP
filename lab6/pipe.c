#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[256];
    time_t parent_time, child_time;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Дочерний процесс
        close(pipefd[1]);
        child_time = time(NULL);
        printf("Child PID: %d, Time: %s", getpid(), ctime(&child_time));
        sleep(5);
        read(pipefd[0], buffer, sizeof(buffer));
        printf("%s", buffer);
        close(pipefd[0]);
    } else { // Родительский процесс
        close(pipefd[0]);
        sleep(5);
        parent_time = time(NULL);
        char message[256];
        snprintf(message, sizeof(message), "Parent PID: %d, Time: %s", getpid(), ctime(&parent_time));
        write(pipefd[1], message, strlen(message) + 1);
        close(pipefd[1]);
    }

    return 0;
}