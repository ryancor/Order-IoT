CC=g++
CFLAGS=-I.
DEPS = helper.hpp
OBJ = main.o helper.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(OBJ)
