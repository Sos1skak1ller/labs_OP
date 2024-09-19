#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <getopt.h>

#define COLOR_DIR "\x1b[34m"   // Синий 
#define COLOR_EXEC "\x1b[32m"  // Зеленый 
#define COLOR_LINK "\x1b[36m"  // Бирюзовый
#define COLOR_RESET "\x1b[0m"  // Сброс цвета

void print_file_info(const char *name, struct stat *file_stat) {

    printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");
    
    printf(" %lu", file_stat->st_nlink);
    struct passwd *pw = getpwuid(file_stat->st_uid);
    struct group *gr = getgrgid(file_stat->st_gid);
    printf(" %s %s", pw->pw_name, gr->gr_name);
    

    printf(" %5lld", (long long)file_stat->st_size);
   
    char timebuf[80];
    struct tm *tm_info = localtime(&file_stat->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
    printf(" %s", timebuf);

    if (S_ISDIR(file_stat->st_mode)) {
        printf(" %s%s%s\n", COLOR_DIR, name, COLOR_RESET);
    } else if (file_stat->st_mode & S_IXUSR) {
        printf(" %s%s%s\n", COLOR_EXEC, name, COLOR_RESET);
    } else if (S_ISLNK(file_stat->st_mode)) {
        printf(" %s%s%s\n", COLOR_LINK, name, COLOR_RESET);
    } else {
        printf(" %s\n", name);
    }
}

void list_directory(const char *path, int show_all, int long_list) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }
    
    struct dirent *entry;
    struct stat file_stat;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        if (stat(fullpath, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (long_list) {
            print_file_info(entry->d_name, &file_stat);
        } else {
            if (S_ISDIR(file_stat.st_mode)) {
                printf("%s%s%s  ", COLOR_DIR, entry->d_name, COLOR_RESET);
            } else if (file_stat.st_mode & S_IXUSR) {
                printf("%s%s%s  ", COLOR_EXEC, entry->d_name, COLOR_RESET);
            } else if (S_ISLNK(file_stat.st_mode)) {
                printf("%s%s%s  ", COLOR_LINK, entry->d_name, COLOR_RESET);
            } else {
                printf("%s  ", entry->d_name);
            }
        }
    }

    if (!long_list) {
        printf("\n");
    }

    closedir(dir);
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
