.PHONY: all clean run

all: ls

ls: main.c
	gcc main.c -o ls -Wall -Wextra

run: ls
	./ls

clean:
	rm -rf *.o ls