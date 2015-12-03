TARGET   = app

CC       = gcc
CFLAGS   = -ansi -pedantic -Wall -Werror -MMD

LINKER   = gcc -o
LFLAGS   = -Wall

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
HEADERS  := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPENDS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.d)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(rm) $(OBJECTS)
	$(rm) $(BINDIR)/$(TARGET)

-include $(DEPENDS)
