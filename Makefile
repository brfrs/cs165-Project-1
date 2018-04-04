CC=gcc
PROJ_NAME=compare

all: MAIN.c COMPARE.c doalg.c
	$(CC) -o $(PROJ_NAME) $<

.PHONY: clean

clean:
	rm -f *.o
