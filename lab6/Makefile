.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra

all: fifo pipe

mycat: mycat.c
	$(CC) $(CFLAGS) fifo.c -o fifo

mygrep: pipe.c
	$(CC) $(CFLAGS) pipe.c -o pipe

clean:
	rm -rf *.o fifo pipe
	
