#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_file(FILE *file, int show_nonempty_line_num, int show_ends) {
    char line[1024];
    int line_num = 1; // Счетчик всех строк
    int non_empty_line_num = 1; // Счетчик непустых строк

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
        } else if (!show_nonempty_line_num) {
            printf("%s", line);
        }

        // Увеличиваем общий счетчик строк, если не показываем номера непустых строк
        if (!show_nonempty_line_num) {
            line_num++;
        }
    }
}

int main(int argc, char *argv[]) {
    int show_nonempty_line_num = 0, show_ends = 0;
    char *filename = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "nEb")) != -1) {
        switch (opt) {
            case 'b':
                show_nonempty_line_num = 1; // Приоритет для непустых строк
                break;
            case 'n':
                if (!show_nonempty_line_num) {
                    show_nonempty_line_num = -1;
                }
                break;
            case 'E':
                show_ends = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n|-b|-E] file\n", argv[0]);
                return 1;
        }
    }

    // Получение имени файла после обработки флагов
    if (optind < argc) {
        filename = argv[optind];
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
    print_file(file, show_nonempty_line_num == 1, show_ends);

    fclose(file);
    return 0;
}
