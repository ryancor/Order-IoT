OS := $(shell uname)
CC=g++
DEPS = pal.hpp
OBJ = pal.o

main: $(OBJ)
ifeq ($(OS),Linux)
	$(CC) -fPIC -c -Wall pal.cpp
	nasm -f elf64 keyread.asm -o keyread.o
	ld -shared keyread.o $(OBJ) -o libpal.so
endif

clean:
	rm -rf $(OBJ) keyread.o
