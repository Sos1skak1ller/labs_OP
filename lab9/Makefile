.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra

all: sender receiver

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

receiver: receiver.c
	$(CC) $(CFLAGS) receiver.c -o receiver

clean:
	rm -rf *.o sender receiver
