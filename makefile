CC = gcc
CFLAGS = -ansi -g -Wall -pedantic
SRCS = structs/list.c helpers/logs.c helpers/memory.c helpers/strings.c helpers/symbols.c syntax/line_parser.c first_pass.c macros.c main.c
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
