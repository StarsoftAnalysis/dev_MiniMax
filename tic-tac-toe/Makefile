EXE=ttt
CC=gcc

all: $(EXE)

%.o: %.c
	$(CC) -std=c99 -c -o $@ $<

$(EXE): nega_tests.o
	$(CC) -o $(EXE) $<

clean:
	rm -rvf *.o $(EXE)
