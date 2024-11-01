#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void add_file_to_archive(const char *arch_name, const char *file_name) {
    int arch_fd = open(arch_name, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (arch_fd < 0) {
        perror("Failed to open archive");
        return;
    }

    int file_fd = open(file_name, O_RDONLY);
    if (file_fd < 0) {
        perror("Failed to open file");
        close(arch_fd);
        return;
    }

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) < 0) {
        perror("Failed to get file stats");
        close(file_fd);
        close(arch_fd);
        return;
    }

    write(arch_fd, &file_stat, sizeof(file_stat));
    write(arch_fd, file_name, strlen(file_name) + 1);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        write(arch_fd, buffer, bytes_read);
    }

    close(file_fd);
    close(arch_fd);
}

void extract_file_from_archive(const char *arch_name, const char *file_name) {
    int arch_fd = open(arch_name, O_RDWR);
    if (arch_fd < 0) {
        perror("Failed to open archive");
        return;
    }

    struct stat file_stat;
    char name_buffer[256];
    ssize_t bytes_read;

    while ((bytes_read = read(arch_fd, &file_stat, sizeof(file_stat))) > 0) {
        read(arch_fd, name_buffer, sizeof(name_buffer));
        if (strcmp(name_buffer, file_name) == 0) {
            int out_fd = open(file_name, O_WRONLY | O_CREAT, file_stat.st_mode);
            if (out_fd < 0) {
                perror("Failed to create output file");
                close(arch_fd);
                return;
            }

            char buffer[BUFFER_SIZE];
            for (off_t i = 0; i < file_stat.st_size; i += BUFFER_SIZE) {
                ssize_t to_read = (file_stat.st_size - i < BUFFER_SIZE) ? file_stat.st_size - i : BUFFER_SIZE;
                read(arch_fd, buffer, to_read);
                write(out_fd, buffer, to_read);
            }
            close(out_fd);

            off_t pos = lseek(arch_fd, -((off_t) sizeof(file_stat) + strlen(name_buffer) + 1 + file_stat.st_size), SEEK_CUR);
            ftruncate(arch_fd, pos);
            break;
        } else {
            lseek(arch_fd, file_stat.st_size, SEEK_CUR);
        }
    }

    close(arch_fd);
}

void print_archive_stat(const char *arch_name) {
    int arch_fd = open(arch_name, O_RDONLY);
    if (arch_fd < 0) {
        perror("Failed to open archive");
        return;
    }

    struct stat file_stat;
    char name_buffer[256];
    ssize_t bytes_read;

    while ((bytes_read = read(arch_fd, &file_stat, sizeof(file_stat))) > 0) {
        read(arch_fd, name_buffer, sizeof(name_buffer));
        printf("File: %s, Size: %ld bytes\n", name_buffer, file_stat.st_size);
        lseek(arch_fd, file_stat.st_size, SEEK_CUR);
    }

    close(arch_fd);
}

int main(int argc, char *argv[]) {
    const char *arch_name = argv[1];

    if (argc == 3 && (strcmp(argv[2], "-h") == 0 || strcmp(argv[2], "--help") == 0)) {
        print_help();
    } else if (argc == 4 && (strcmp(argv[2], "-i") == 0 || strcmp(argv[2], "--input") == 0)) {
        add_file_to_archive(arch_name, argv[3]);
    } else if (argc == 4 && (strcmp(argv[2], "-e") == 0 || strcmp(argv[2], "--extract") == 0)) {
        extract_file_from_archive(arch_name, argv[3]);
    } else if (argc == 3 && (strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "--stat") == 0)) {
        print_archive_stat(arch_name);
    } else {
        print_help();
    }

    return 0;
}
