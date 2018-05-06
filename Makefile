OS := $(shell uname)
CC=g++
CFLAGS=-lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -I.
DEPS = helper.hpp requests.hpp ip.hpp
OBJ = main.o helper.o requests.o ip.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	ifeq ($(OS),Linux)
		$(CC) -fPIC -c -Wall -shared lib/pal.cpp -o lib/libpal.so
	endif

main: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(OBJ)
