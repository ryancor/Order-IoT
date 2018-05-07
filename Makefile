OS := $(shell uname)
CC=g++
ifeq ($(OS),Linux)
	CFLAGS=-lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -I. -Wall -L $(PWD)/lib, -rpath=$(PWD)/lib/ -lpal
else
	CFLAGS=-lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -I.
endif
DEPS = helper.hpp requests.hpp ip.hpp
OBJ = main.o helper.o requests.o ip.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(OBJ)
