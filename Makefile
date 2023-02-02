CC=gcc

CXXFLAGS = -g -lm

proj1A: proj1A.o
	$(CC) -o proj1A_main proj1A.o 

proj1B: proj1B.o
	$(CC) -o proj1B_main proj1B.o $(CXXFLAGS)

proj1C: proj1C.o
	$(CC) -o proj1C_main proj1C.o $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f *.o *main *.png *_out.pnm
