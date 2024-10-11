#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024

// Функция для поиска шаблона в строке
int match_pattern(const char *line, const char *pattern) {
    return strstr(line, pattern) != NULL;
}

// Основная функция для поиска шаблона в файле или потоке
void grep_in_file(const char *pattern, FILE *file) {
    char line[MAX_LINE_LEN];

    // Читаем строки по одной и ищем совпадения
    while (fgets(line, MAX_LINE_LEN, file) != NULL) {
        if (match_pattern(line, pattern)) {
            printf("%s", line);
            fflush(stdout);  // Сбрасываем буфер вывода
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s pattern [file]\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[1];
    FILE *file = NULL;

    // Проверка, есть ли файл в аргументах
    if (argc == 3) {
        file = fopen(argv[2], "r");
        if (file == NULL) {
            perror("fopen");
            return 1;
        }
    } else {
        // Если файл не указан, читаем данные из stdin (поддержка каналов)
        file = stdin;
    }

    // Поиск шаблона в файле или stdin
    grep_in_file(pattern, file);

    if (file != stdin) {
        fclose(file);
    }

    return 0;
}
