CC=g++
CXXFLAGS = -lglfw3 -lGL -lm -lX11 -lpthread -lXi -lXrandr -ldl

main: main.o
	$(CC) -o main $(CFLAGS) main.o 

.PHONY: clean

clean:
	rm -f *.o main
