#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_file(FILE *file, int show_line_num, int show_nonempty_line_num, int show_ends) {
    char line[1024];
    int line_num = 1;
    int non_empty_line_num = 1;
    
    while (fgets(line, sizeof(line), file)) {
        int is_non_empty = strlen(line) > 1; // Проверка на непустую строку

        // Добавляем символ '$' в конце строки, если установлен флаг show_ends
        if (show_ends) {
            line[strcspn(line, "\n")] = '\0'; // Удаление символа новой строки
            strcat(line, "$\n");
        }

        // Вывод строк в зависимости от установленных флагов
        if (show_nonempty_line_num && is_non_empty) {
            printf("%6d\t%s", non_empty_line_num++, line);
        } else if (show_line_num) {
            printf("%6d\t%s", line_num++, line);
        } else {
            printf("%s", line);
        }
    }
}

int main(int argc, char *argv[]) {
    int show_line_num = 0, show_nonempty_line_num = 0, show_ends = 0;
    char *filename = NULL;

    // Обработка аргументов командной строки
    for (int i = 1; i < argc; i++) {
        switch (argv[i][0]) {
            case '-':
                switch (argv[i][1]) {
                    case 'n':
                        show_line_num = 1;
                        break;
                    case 'b':
                        show_nonempty_line_num = 1;
                        break;
                    case 'E':
                        show_ends = 1;
                        break;
                    default:
                        fprintf(stderr, "Unknown option: %s\n", argv[i]);
                        return 1;
                }
                break;
            default:
                filename = argv[i];
                break;
        }
    }

    // Проверка на наличие файла в аргументах
    if (filename == NULL) {
        fprintf(stderr, "Usage: %s [-n|-b|-E] file\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Печать содержимого файла с учётом флагов
    print_file(file, show_line_num, show_nonempty_line_num, show_ends);

    fclose(file);
    return 0;
}
