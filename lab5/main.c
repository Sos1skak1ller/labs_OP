//  Реализовать примитивный архиватор.  Примечания:
// без сжатия;
// через системные вызовы open, read, write, lseek;
// необходимо предусмотреть удаление файлов из архива. Либо отдельным флагом, либо через -e – файл извлекается из архива, и в архиве больше не находится;
// все атрибуты помещаемого в архив файла должны сохраняться (при извлечении все атрибуты должны оказаться теми же, что и до помещения в архив). 
// Примеры использования:
// ./archiver arch_name –i(--input) file1
// ./archiver arch_name –e(--extract) file1
// ./archiver arch_name –s(--stat) // вывести текущее состояние архива 
// ./archiver –h(--help) // вывести справку по работе с архиватором


// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/stat.h>
// #include <errno.h>

// #define HEADER_SIZE 256

// typedef struct {
//     char name[128];
//     mode_t mode;
//     off_t size;
// } FileHeader;

// void print_help() {
//     printf("Usage:\n");
//     printf("  ./archiver arch_name -i file1\n");
//     printf("  ./archiver arch_name -e file1\n");
//     printf("  ./archiver arch_name -s\n");
//     printf("  ./archiver -h\n");
// }

// void add_file_to_archive(const char *archive_name, const char *file_name) {
//     int arch_fd = open(archive_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
//     if (arch_fd < 0) {
//         perror("Error opening archive");
//         return;
//     }

//     int file_fd = open(file_name, O_RDONLY);
//     if (file_fd < 0) {
//         perror("Error opening input file");
//         close(arch_fd);
//         return;
//     }

//     struct stat st;
//     if (fstat(file_fd, &st) < 0) {
//         perror("Error getting file attributes");
//         close(file_fd);
//         close(arch_fd);
//         return;
//     }

//     FileHeader header;
//     memset(&header, 0, sizeof(FileHeader));
//     strncpy(header.name, file_name, sizeof(header.name) - 1);
//     header.mode = st.st_mode;
//     header.size = st.st_size;

//     if (write(arch_fd, &header, sizeof(FileHeader)) != sizeof(FileHeader)) {
//         perror("Error writing file header");
//         close(file_fd);
//         close(arch_fd);
//         return;
//     }

//     char buffer[1024];
//     ssize_t bytes_read;
//     while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
//         if (write(arch_fd, buffer, bytes_read) != bytes_read) {
//             perror("Error writing file content");
//             close(file_fd);
//             close(arch_fd);
//             return;
//         }
//     }

//     if (bytes_read < 0) {
//         perror("Error reading file content");
//     }

//     close(file_fd);
//     close(arch_fd);
// }

// void extract_file_from_archive(const char *archive_name, const char *file_name) {
//     int arch_fd = open(archive_name, O_RDONLY);
//     if (arch_fd < 0) {
//         perror("Error opening archive");
//         return;
//     }

//     FileHeader header;
//     while (read(arch_fd, &header, sizeof(FileHeader)) == sizeof(FileHeader)) {
//         if (strcmp(header.name, file_name) == 0) {
//             int out_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, header.mode);
//             if (out_fd < 0) {
//                 perror("Error creating output file");
//                 close(arch_fd);
//                 return;
//             }

//             char buffer[1024];
//             ssize_t bytes_to_read = header.size;
//             while (bytes_to_read > 0) {
//                 ssize_t chunk = read(arch_fd, buffer, (bytes_to_read > sizeof(buffer)) ? sizeof(buffer) : bytes_to_read);
//                 if (chunk <= 0) {
//                     perror("Error reading file content");
//                     close(out_fd);
//                     close(arch_fd);
//                     return;
//                 }

//                 if (write(out_fd, buffer, chunk) != chunk) {
//                     perror("Error writing to output file");
//                     close(out_fd);
//                     close(arch_fd);
//                     return;
//                 }
//                 bytes_to_read -= chunk;
//             }

//             close(out_fd);
//             printf("File '%s' extracted successfully.\n", file_name);
//             close(arch_fd);
//             return;
//         } else {
//             lseek(arch_fd, header.size, SEEK_CUR);
//         }
//     }

//     printf("File '%s' not found in archive.\n", file_name);
//     close(arch_fd);
// }

// void print_archive_status(const char *archive_name) {
//     int arch_fd = open(archive_name, O_RDONLY);
//     if (arch_fd < 0) {
//         perror("Error opening archive");
//         return;
//     }

//     FileHeader header;
//     while (read(arch_fd, &header, sizeof(FileHeader)) == sizeof(FileHeader)) {
//         printf("File: %s, Size: %ld bytes, Mode: %o\n", header.name, header.size, header.mode);
//         lseek(arch_fd, header.size, SEEK_CUR);
//     }

//     close(arch_fd);
// }

// int main(int argc, char *argv[]) {
//     if (argc < 2) {
//         print_help();
//         return 1;
//     }

//     if (strcmp(argv[1], "-h") == 0) {
//         print_help();
//         return 0;
//     }

//     if (argc < 3) {
//         print_help();
//         return 1;
//     }

//     const char *archive_name = argv[1];
//     const char *command = argv[2];

//     if (strcmp(command, "-i") == 0) {
//         if (argc < 4) {
//             fprintf(stderr, "Error: No input file specified.\n");
//             return 1;
//         }
//         add_file_to_archive(archive_name, argv[3]);
//     } else if (strcmp(command, "-e") == 0) {
//         if (argc < 4) {
//             fprintf(stderr, "Error: No file to extract specified.\n");
//             return 1;
//         }
//         extract_file_from_archive(archive_name, argv[3]);
//     } else if (strcmp(command, "-s") == 0) {
//         print_archive_status(archive_name);
//     } else {
//         print_help();
//     }

//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>

#define BLOCK_SIZE 4096

struct file_header {
    char filename[256];
    mode_t mode;
    off_t size;
    time_t mtime;
};

void print_help() {
    printf("Usage:\n"
           "./archiver arch_name -i|--input file    # Add file to archive\n"
           "./archiver arch_name -e|--extract file  # Extract file from archive\n"
           "./archiver arch_name -s|--stat          # Show archive statistics\n"
           "./archiver -h|--help                    # Show this help\n");
}

int add_to_archive(const char *archive_name, const char *filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("stat");
        return -1;
    }

    int arch_fd = open(archive_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (arch_fd == -1) {
        perror("open archive");
        return -1;
    }

    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        perror("open file");
        close(arch_fd);
        return -1;
    }

    struct file_header header = {0};
    strncpy(header.filename, filename, sizeof(header.filename) - 1);
    header.mode = st.st_mode;
    header.size = st.st_size;
    header.mtime = st.st_mtime;

    if (write(arch_fd, &header, sizeof(header)) != sizeof(header)) {
        perror("write header");
        goto cleanup;
    }

    char buf[BLOCK_SIZE];
    ssize_t n;
    while ((n = read(file_fd, buf, BLOCK_SIZE)) > 0) {
        if (write(arch_fd, buf, n) != n) {
            perror("write data");
            goto cleanup;
        }
    }

cleanup:
    close(file_fd);
    close(arch_fd);
    return 0;
}

int extract_from_archive(const char *archive_name, const char *filename) {
    int arch_fd = open(archive_name, O_RDONLY);
    if (arch_fd == -1) {
        perror("open archive");
        return -1;
    }

    struct file_header header;
    while (read(arch_fd, &header, sizeof(header)) == sizeof(header)) {
        if (strcmp(header.filename, filename) == 0) {
            int file_fd = open(header.filename, O_WRONLY | O_CREAT | O_TRUNC, header.mode);
            if (file_fd == -1) {
                perror("create file");
                close(arch_fd);
                return -1;
            }

            char buf[BLOCK_SIZE];
            off_t remaining = header.size;
            while (remaining > 0) {
                ssize_t to_read = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
                ssize_t n = read(arch_fd, buf, to_read);
                if (n <= 0) break;
                if (write(file_fd, buf, n) != n) {
                    perror("write file");
                    break;
                }
                remaining -= n;
            }

            struct timespec times[2] = {{0, 0}, {header.mtime, 0}};
            utimensat(AT_FDCWD, header.filename, times, 0);
            close(file_fd);
            close(arch_fd);
            return 0;
        }
        lseek(arch_fd, header.size, SEEK_CUR);
    }

    close(arch_fd);
    fprintf(stderr, "File not found in archive\n");
    return -1;
}

void show_stats(const char *archive_name) {
    int fd = open(archive_name, O_RDONLY);
    if (fd == -1) {
        perror("open archive");
        return;
    }

    printf("Archive contents:\n");
    struct file_header header;
    while (read(fd, &header, sizeof(header)) == sizeof(header)) {
        printf("%s (size: %ld bytes)\n", header.filename, header.size);
        lseek(fd, header.size, SEEK_CUR);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"extract", required_argument, 0, 'e'},
        {"stat", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    if (argc < 2) {
        print_help();
        return 1;
    }

    int option;
    while ((option = getopt_long(argc, argv, "i:e:sh", long_options, 0)) != -1) {
        switch (option) {
            case 'i':
                return add_to_archive(argv[1], optarg);
            case 'e':
                return extract_from_archive(argv[1], optarg);
            case 's':
                show_stats(argv[1]);
                return 0;
            case 'h':
                print_help();
                return 0;
            default:
                print_help();
                return 1;
        }
    }

    return 0;
}