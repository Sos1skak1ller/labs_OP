#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <mode> <file>\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *filepath = argv[2];
    struct stat st;
    mode_t current_permissions;

    if (stat(filepath, &st) < 0) {
        fprintf(stderr, "Error getting permissions for %s: %s\n",
                filepath, strerror(errno));
        return 1;
    }

    current_permissions = st.st_mode;

    if (mode[0] == '+' || mode[0] == '-' || strchr(mode, '=') ||
        (mode[0] == 'u' || mode[0] == 'g' || mode[0] == 'o' || mode[0] == 'a')) {
        
        char op = '\0';
        mode_t chmod_mode = 0;
        const char *p = mode;
    
        if (mode[0] == 'u' || mode[0] == 'g' || mode[0] == 'o' || mode[0] == 'a') {
            p++;
        }
        
        op = *p++;
        
        while (*p) {
            switch (*p) {
                case 'r': chmod_mode |= S_IRUSR | S_IRGRP | S_IROTH; break;
                case 'w': chmod_mode |= S_IWUSR | S_IWGRP | S_IWOTH; break;
                case 'x': chmod_mode |= S_IXUSR | S_IXGRP | S_IXOTH; break;
            }
            p++;
        }

        if (mode[0] == 'u') chmod_mode &= S_IRWXU;
        else if (mode[0] == 'g') chmod_mode &= S_IRWXG;
        else if (mode[0] == 'o') chmod_mode &= S_IRWXO;

        if (op == '+') {
            current_permissions |= chmod_mode;
        } else if (op == '-') {
            current_permissions &= ~chmod_mode;
        } else if (op == '=') {
            current_permissions = chmod_mode;
        }
    } else {
        char *endptr;
        long new_mode = strtol(mode, &endptr, 8);
        
        if (*endptr != '\0' || new_mode < 0 || new_mode > 0777) {
            fprintf(stderr, "Error: Invalid octal mode %s\n", mode);
            return 1;
        }
        
        current_permissions = (current_permissions & ~(S_IRWXU | S_IRWXG | S_IRWXO)) | new_mode;
    }

    if (chmod(filepath, current_permissions) < 0) {
        fprintf(stderr, "Error changing permissions for %s: %s\n", 
                filepath, strerror(errno));
        return 1;
    }

    return 0;
}