#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <ctype.h> 

#define COLOR_DIR "\x1b[34m"   // Синий
#define COLOR_EXEC "\x1b[32m"  // Зеленый
#define COLOR_LINK "\x1b[35m"  // Розовый
#define COLOR_RESET "\x1b[0m"  // Сброс цвета

typedef struct {
    char name[256];
    struct stat file_stat;
} file_info_t;

void print_file_info(const char *fullpath, const char *name, struct stat *file_stat) {
    // Права доступа
    printf((S_ISDIR(file_stat->st_mode)) ? "d" : (S_ISLNK(file_stat->st_mode)) ? "l" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");

    // Количество жестких ссылок
    printf(" %3hu", file_stat->st_nlink);

    // Имя владельца и группы с проверкой
    struct passwd *pw = getpwuid(file_stat->st_uid);
    if (pw) {
        printf(" %-5s", pw->pw_name);
    } else {
        printf(" %-5u", file_stat->st_uid);
    }

    struct group *gr = getgrgid(file_stat->st_gid);
    if (gr) {
        printf(" %-5s", gr->gr_name);
    } else {
        printf(" %-5u", file_stat->st_gid);
    }

    // Размер файла
    printf(" %5lld", (long long)file_stat->st_size);

    // Время изменения файла
    char timebuf[80];
    struct tm *tm_info = localtime(&file_stat->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
    printf(" %s", timebuf);

    // Обработка символической ссылки
    if (S_ISLNK(file_stat->st_mode)) {
        char link_target[1024];
        ssize_t len = readlink(fullpath, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(" %s%s%s -> %s\n", COLOR_LINK, name, COLOR_RESET, link_target);
        } else {
            printf(" %s%s%s\n", COLOR_LINK, name, COLOR_RESET);
        }
    } else if (S_ISDIR(file_stat->st_mode)) {
        printf(" %s%s%s\n", COLOR_DIR, name, COLOR_RESET);
    } else if (file_stat->st_mode & S_IXUSR) {
        printf(" %s%s%s\n", COLOR_EXEC, name, COLOR_RESET);
    } else {
        printf(" %s\n", name);
    }
}

// Функция сравнения для сортировки в стиле ls
int compare_files(const void *a, const void *b) {
    const file_info_t *fileA = (const file_info_t *)a;
    const file_info_t *fileB = (const file_info_t *)b;

    // Если имена файлов начинаются с точки, сортируем их первыми
    if (fileA->name[0] == '.' && fileB->name[0] != '.') {
        return -1;
    }
    if (fileA->name[0] != '.' && fileB->name[0] == '.') {
        return 1;
    }

    // Сравниваем строки, сначала по регистру (заглавные, потом строчные)
    if (isupper(fileA->name[0]) && islower(fileB->name[0])) {
        return -1;
    }
    if (islower(fileA->name[0]) && isupper(fileB->name[0])) {
        return 1;
    }

    // Стандартное сравнение строк без учета регистра
    return strcasecmp(fileA->name, fileB->name);
}

void list_directory(const char *path, int show_all, int long_list) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    file_info_t *files = NULL;
    size_t count = 0;
    struct dirent *entry;
    long long total_blocks = 0;

    // Сбор информации о файлах
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        files = realloc(files, (count + 1) * sizeof(file_info_t));
        strncpy(files[count].name, entry->d_name, sizeof(files[count].name) - 1);
        files[count].name[sizeof(files[count].name) - 1] = '\0';

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        // Используем lstat для корректной обработки символических ссылок
        if (lstat(fullpath, &files[count].file_stat) == -1) {
            perror("lstat");
            free(files);
            closedir(dir);
            return;
        }

        // Считаем общее количество блоков
        total_blocks += files[count].file_stat.st_blocks;

        count++;
    }

    closedir(dir);

    // Вывод общего количества блоков
    if (long_list) {
        printf("total %lld\n", total_blocks);
    }

    // Сортировка файлов
    qsort(files, count, sizeof(file_info_t), compare_files);

    // Вывод информации о файлах
    for (size_t i = 0; i < count; i++) {
        if (long_list) {
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[i].name);
            print_file_info(fullpath, files[i].name, &files[i].file_stat);
        } else {
            if (S_ISDIR(files[i].file_stat.st_mode)) {
                printf("%s%s%s  ", COLOR_DIR, files[i].name, COLOR_RESET);
            } else if (files[i].file_stat.st_mode & S_IXUSR) {
                printf("%s%s%s  ", COLOR_EXEC, files[i].name, COLOR_RESET);
            } else if (S_ISLNK(files[i].file_stat.st_mode)) {
                printf("%s%s%s  ", COLOR_LINK, files[i].name, COLOR_RESET);
            } else {
                printf("%s  ", files[i].name);
            }
        }
    }

    if (!long_list) {
        printf("\n");
    }

    free(files);
}

int main(int argc, char *argv[]) {
    int opt;
    int show_all = 0, long_list = 0;

    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'l':
                long_list = 1;
                break;
            case 'a':
                show_all = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-a] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    list_directory(path, show_all, long_list);

    return 0;
}
