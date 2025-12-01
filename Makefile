CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -pedantic
SRCS = main.c parse.c builtin.c jobs.c
TARGET = ebsh

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
