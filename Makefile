CC=gcc
PROJ_NAME=compare
DEBUG=-g

all: MAIN.c COMPARE.c doalg.c
	$(CC) $(DEBUG) -o $(PROJ_NAME) $<

.PHONY: clean

clean:
	rm -f *.o
