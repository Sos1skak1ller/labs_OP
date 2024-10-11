.PHONY: all clean run

all: mychmod

fork: main.c
	gcc main.c -o fork -Wall -Wextra

run: mychmod
	./mychmod

clean:
	rm -rf *.o mychmod