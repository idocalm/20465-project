CC = gcc
CFLAGS = -ansi -g -Wall -pedantic
SRCS = structs/list.c structs/labels.c helpers/logs.c helpers/memory.c helpers/strings.c helpers/symbols.c syntax/code_line.c syntax/data_line.c second_pass.c first_pass.c macros.c output.c assembler.c
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
