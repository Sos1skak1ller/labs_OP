.PHONY: all clean run

all: fork

fork: main.c
	gcc main.c -o fork -Wall -Wextra

run: fork
	./fork

clean:
	rm -rf *.o fork