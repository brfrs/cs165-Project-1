CC=gcc
PROJ_NAME=compare
DEBUG=-g
OPTIONS=-Wno-implicit-int

all: MAIN.c COMPARE.c doalg.c
	$(CC) $(OPTIONS) $(DEBUG) -o $(PROJ_NAME) $<

.PHONY: clean

clean:
	rm -f *.o
