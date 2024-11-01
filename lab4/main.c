#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

void change_permissions(const char *mode, const char *filepath) {
    struct stat file_stat;

    // Получаем текущие права доступа к файлу
    if (stat(filepath, &file_stat) < 0) {
        perror("Ошибка получения информации о файле");
        exit(EXIT_FAILURE);
    }

    mode_t current_permissions = file_stat.st_mode;

    // Обрабатываем режимы
    if (mode[0] == '+') {
        for (size_t i = 1; i < strlen(mode); i++) {
            switch (mode[i]) {
                case 'r':
                    current_permissions |= S_IRUSR | S_IRGRP | S_IROTH;
                    break;
                case 'w':
                    current_permissions |= S_IWUSR | S_IWGRP | S_IWOTH;
                    break;
                case 'x':
                    current_permissions |= S_IXUSR | S_IXGRP | S_IXOTH;
                    break;
            }
        }
    } else if (mode[0] == '-') {
        for (size_t i = 1; i < strlen(mode); i++) {
            switch (mode[i]) {
                case 'r':
                    current_permissions &= ~(S_IRUSR | S_IRGRP | S_IROTH);
                    break;
                case 'w':
                    current_permissions &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
                    break;
                case 'x':
                    current_permissions &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
                    break;
            }
        }
    } else {
        // Установка прав в формате 766
        int new_mode = strtol(mode, NULL, 8);
        current_permissions = (current_permissions & ~S_IRWXU & ~S_IRWXG & ~S_IRWXO) | new_mode;
    }

    // Устанавливаем новые права доступа к файлу
    if (chmod(filepath, current_permissions) < 0) {
        perror("Ошибка изменения прав доступа к файлу");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <mode> <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *mode = argv[1];
    const char *filepath = argv[2];

    change_permissions(mode, filepath);
    return EXIT_SUCCESS;
}
