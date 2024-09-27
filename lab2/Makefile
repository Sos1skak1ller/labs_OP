.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra

all: mycat mygrep

mycat: mycat.c
	$(CC) $(CFLAGS) mycat.c -o mycat

mygrep: mygrep.c
	$(CC) $(CFLAGS) mygrep.c -o mygrep

clean:
	rm -rf *.o mycat mygrep
