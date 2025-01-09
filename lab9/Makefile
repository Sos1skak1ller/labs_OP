.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SEMFILE = semfile

all: sender receiver $(SEMFILE)

$(SEMFILE):
	touch $(SEMFILE)

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c  -pthread

receiver: receiver.c
	$(CC) $(CFLAGS) -o receiver receiver.c  -pthread

clean:	
	rm -f sender receiver sender.lock shmfile $(SEMFILE)