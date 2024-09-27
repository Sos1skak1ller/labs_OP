#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void grep_pattern(FILE *file, const char *pattern) {
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, pattern)) {
            printf("%s", line);
        }
    }
}

int main(int argc, char *argv[]) {
    char *pattern = NULL;
    char *filename = NULL;

    if (argc == 2) {
        pattern = argv[1];
        grep_pattern(stdin, pattern);
    } else if (argc == 3) {
        pattern = argv[1];
        filename = argv[2];
    } else {
        fprintf(stderr, "Usage: %s pattern [file]\n", argv[0]);
        return 1;
    }

    if (filename) {
        FILE *file = fopen(filename, "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }

        grep_pattern(file, pattern);
        fclose(file);
    }

    return 0;
}
