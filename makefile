CC=gcc
CFLAGS=-ansi -pedantic -Wall -Werror -g
SOURCES=main.c bitmap.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=app

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
