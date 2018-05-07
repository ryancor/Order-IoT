OS := $(shell uname)
CC=g++
DEPS = pal.hpp
OBJ = pal.o

main: $(OBJ)
	ifeq ($(OS),Linux)
		$(CC) -fPIC -c -Wall pal.cpp
		ld -shared $(OBJ) -o libpal.so
	endif

clean:
	rm -rf $(OBJ)
