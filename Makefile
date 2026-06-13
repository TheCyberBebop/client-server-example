# Compiler and flags
CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic
DBGFLAGS := -g -DDEBUG

# Source file
SRC := server.c

# Targets
all: server_debug server_release

server_debug: $(SRC)
	$(CC) $(CFLAGS) $(DBGFLAGS) -o $@ $^

server_release: $(SRC)
	$(CC) $(CFLAGS) -O2 -o $@ $^

clean:
	rm -f server_debug server_release *.o

.PHONY: all clean server_debug server_release