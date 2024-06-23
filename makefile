CC = gcc
CFLAGS = -ansi -Wall -pedantic
SRCS = structs/*.c syntax/*.c *.c
OBJS = $(SRCS:.c=.o)
TARGET = asm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
