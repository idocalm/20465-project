CC = gcc
CFLAGS = -ansi -Wall -pedantic
SRCS = structs/hash_table.c helpers/logs.c helpers/memory.c helpers/strings.c helpers/symbols.c syntax/commands.c first_pass.c macros.c main.c
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
