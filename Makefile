CC=gcc

proj1A: proj1A.o
	$(CC) -o 1A_main proj1A.o 

proj1B: proj1B.o
	$(CC) -o 1B_main proj1B.o 

.PHONY: clean

clean:
	rm -f *.o 1A_main proj1A_out.png proj1A_out.pnm 1B_main proj1B_out.png proj1B_out.pnm
