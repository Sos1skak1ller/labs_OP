#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define HEADER_SIZE 256

typedef struct {
    char name[128];
    mode_t mode;
    off_t size;
} FileHeader;

void print_help() {
    printf("Usage:\n");
    printf("  ./archiver arch_name -i file1\n");
    printf("  ./archiver arch_name -e file1\n");
    printf("  ./archiver arch_name -s\n");
    printf("  ./archiver -h\n");
}

void add_file_to_archive(const char *archive_name, const char *file_name) {
    int arch_fd = open(archive_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (arch_fd < 0) {
        perror("Error opening archive");
        return;
    }

    int file_fd = open(file_name, O_RDONLY);
    if (file_fd < 0) {
        perror("Error opening input file");
        close(arch_fd);
        return;
    }

    struct stat st;
    if (fstat(file_fd, &st) < 0) {
        perror("Error getting file attributes");
        close(file_fd);
        close(arch_fd);
        return;
    }

    FileHeader header;
    memset(&header, 0, sizeof(FileHeader));
    strncpy(header.name, file_name, sizeof(header.name) - 1);
    header.mode = st.st_mode;
    header.size = st.st_size;

    if (write(arch_fd, &header, sizeof(FileHeader)) != sizeof(FileHeader)) {
        perror("Error writing file header");
        close(file_fd);
        close(arch_fd);
        return;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        if (write(arch_fd, buffer, bytes_read) != bytes_read) {
            perror("Error writing file content");
            close(file_fd);
            close(arch_fd);
            return;
        }
    }

    if (bytes_read < 0) {
        perror("Error reading file content");
    }

    close(file_fd);
    close(arch_fd);
}

void extract_file_from_archive(const char *archive_name, const char *file_name) {
    int arch_fd = open(archive_name, O_RDONLY);
    if (arch_fd < 0) {
        perror("Error opening archive");
        return;
    }

    FileHeader header;
    while (read(arch_fd, &header, sizeof(FileHeader)) == sizeof(FileHeader)) {
        if (strcmp(header.name, file_name) == 0) {
            int out_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, header.mode);
            if (out_fd < 0) {
                perror("Error creating output file");
                close(arch_fd);
                return;
            }

            char buffer[1024];
            ssize_t bytes_to_read = header.size;
            while (bytes_to_read > 0) {
                ssize_t chunk = read(arch_fd, buffer, (bytes_to_read > sizeof(buffer)) ? sizeof(buffer) : bytes_to_read);
                if (chunk <= 0) {
                    perror("Error reading file content");
                    close(out_fd);
                    close(arch_fd);
                    return;
                }

                if (write(out_fd, buffer, chunk) != chunk) {
                    perror("Error writing to output file");
                    close(out_fd);
                    close(arch_fd);
                    return;
                }
                bytes_to_read -= chunk;
            }

            close(out_fd);
            printf("File '%s' extracted successfully.\n", file_name);
            close(arch_fd);
            return;
        } else {
            lseek(arch_fd, header.size, SEEK_CUR);
        }
    }

    printf("File '%s' not found in archive.\n", file_name);
    close(arch_fd);
}

void print_archive_status(const char *archive_name) {
    int arch_fd = open(archive_name, O_RDONLY);
    if (arch_fd < 0) {
        perror("Error opening archive");
        return;
    }

    FileHeader header;
    while (read(arch_fd, &header, sizeof(FileHeader)) == sizeof(FileHeader)) {
        printf("File: %s, Size: %ld bytes, Mode: %o\n", header.name, header.size, header.mode);
        lseek(arch_fd, header.size, SEEK_CUR);
    }

    close(arch_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    if (argc < 3) {
        print_help();
        return 1;
    }

    const char *archive_name = argv[1];
    const char *command = argv[2];

    if (strcmp(command, "-i") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Error: No input file specified.\n");
            return 1;
        }
        add_file_to_archive(archive_name, argv[3]);
    } else if (strcmp(command, "-e") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Error: No file to extract specified.\n");
            return 1;
        }
        extract_file_from_archive(archive_name, argv[3]);
    } else if (strcmp(command, "-s") == 0) {
        print_archive_status(archive_name);
    } else {
        print_help();
    }

    return 0;
}
