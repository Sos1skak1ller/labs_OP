.PHONY: all clean run

all: mychmod

mychmod: main.c
	gcc main.c -o mychmod -Wall -Wextra

run: mychmod
	./mychmod

clean:
	rm -rf *.o mychmod