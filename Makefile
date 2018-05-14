OS := $(shell uname)
CC=g++
ifeq ($(OS),Linux)
	CFLAGS=-lsfml-network -lsfml-system -I. -Wall -L $(PWD)/lib -Wl,-rpath=$(PWD)/lib/ -lpal
else
	CFLAGS=-lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -I.
endif
DEPS = src/helper.hpp src/requests.hpp src/ip.hpp src/sql.hpp
OBJ = src/main.o src/helper.o src/requests.o src/ip.o src/sql.o

%.o: %.c $(DEPS)
	$(CC) `mysql_config --cflags` -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) `mysql_config --libs`

clean:
	rm -rf $(OBJ)
