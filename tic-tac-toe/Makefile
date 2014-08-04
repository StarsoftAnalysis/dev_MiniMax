EXE=ttt
CC=gcc

all: $(EXE)

%.o: %.c
	$(CC) -std=c99 -g -c -o $@ $< -I.

$(EXE): nega_tests.o
	$(CC) -o $(EXE) $< -lm -g

clean:
	rm -rvf *.o $(EXE)
