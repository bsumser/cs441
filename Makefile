CC=gcc

1A: 1A.o
	$(CC) -o 1A_main 1A.o 

.PHONY: clean

clean:
	rm -f *.o 1A_main out.png out.pnm
