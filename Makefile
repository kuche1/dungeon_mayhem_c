# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#combine

EXECUTABLE := dungeon_mayhem

FILES_C := $(wildcard *.c)
FILES_H := $(FILES_C:.c=.h) # an h file for each c file

CC := gcc

CFLAGS := -std=c99
CFLAGS += -Werror -Wextra -Wall -pedantic -Wfatal-errors -Wshadow

.PHONY: all
all: $(EXECUTABLE)

.PHONY: clean
clean:
	rm -f $(EXECUTABLE)

$(EXECUTABLE): Makefile $(FILES_C) $(FILES_H)
	$(CC) $(CFLGAS) -o $(EXECUTABLE) $(FILES_C) $(FILES_H)
