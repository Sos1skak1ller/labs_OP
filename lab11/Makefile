CC = gcc

CFLAGS = -Wall -pthread

TARGET = main

SRC = make.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
