OS := $(shell uname)
CC=g++
ifeq ($(OS),Linux)
	CFLAGS=-lsfml-network -lsfml-system -I. -Wall -L $(PWD)/lib -Wl,-rpath=$(PWD)/lib/ -lpal
else
	CFLAGS=-lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -I.
endif
DEPS = include/helper.hpp include/requests.hpp include/ip.hpp include/sql.hpp include/encode.hpp
OBJ = src/main.o src/helper.o src/requests.o src/ip.o src/sql.o src/encode.o

%.o: %.c $(DEPS)
	$(CC) `mysql_config --cflags` -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) `mysql_config --libs`

clean:
	rm -rf $(OBJ)
