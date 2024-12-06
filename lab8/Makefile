.PHONY: all clean run

all: read_write

read_write: main.c
	gcc main.c -o read_write -Wall -Wextra

run: read_write
	./read_write

clean:
	rm -rf *.o read_write