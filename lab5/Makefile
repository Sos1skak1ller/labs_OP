.PHONY: all clean run

all: archiver

archiver: main.c
	gcc main.c -o archiver -Wall -Wextra

run: archiver
	./archiver

clean:
	rm -rf *.o archiver