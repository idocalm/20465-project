CC = gcc
CFLAGS = -ansi -Wall -pedantic
SRCS = structs/hash_table.c syntax/helpers.c syntax/symbols.c first_pass.c macros.c main.c
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
