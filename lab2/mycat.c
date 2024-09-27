#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_file(FILE *file, int show_line_num, int show_nonempty_line_num, int show_ends) {
    char line[1024];
    int line_num = 1;
    int non_empty_line_num = 1;
    
    while (fgets(line, sizeof(line), file)) {
        if (show_ends) {
            line[strcspn(line, "\n")] = '\0'; // Удаление символа новой строки
            strcat(line, "$\n");
        }
        
        if (show_line_num) {
            printf("%6d\t%s", line_num++, line);
        } else if (show_nonempty_line_num && strlen(line) > 1) {
            printf("%6d\t%s", non_empty_line_num++, line);
        } else {
            printf("%s", line);
        }
    }
}

int main(int argc, char *argv[]) {
    int show_line_num = 0, show_nonempty_line_num = 0, show_ends = 0;
    char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            show_line_num = 1;
        } else if (strcmp(argv[i], "-b") == 0) {
            show_nonempty_line_num = 1;
        } else if (strcmp(argv[i], "-E") == 0) {
            show_ends = 1;
        } else {
            filename = argv[i];
        }
    }

    if (filename == NULL) {
        fprintf(stderr, "Usage: %s [-n|-b|-E] file\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    print_file(file, show_line_num, show_nonempty_line_num, show_ends);

    fclose(file);
    return 0;
}
