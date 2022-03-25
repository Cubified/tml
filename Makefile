all: tml

CC=cc

LIBS=
CFLAGS=-O3 -pipe
DEBUGCFLAGS=-Og -pipe -g

INPUT=tml.c
OUTPUT=tml

RM=/bin/rm

tml: tml.c globals.h
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)
debug: tml.c globals.h
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)
clean: tml
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
