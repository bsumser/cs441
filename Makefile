CC=gcc

proj1A: proj1A.o
	$(CC) -o 1A_main proj1A.o 

.PHONY: clean

clean:
	rm -f *.o 1A_main proj1A_out.png proj1A_out.pnm
