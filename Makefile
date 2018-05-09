OS := $(shell uname)
CC=g++
ifeq ($(OS),Linux)
	CFLAGS=-lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -I. -L  -Wall -L $(PWD)/lib -Wl, -rpath=$(PWD)/lib/ -lpal
else
	CFLAGS=-lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -I.
endif
DEPS = helper.hpp requests.hpp ip.hpp sql.hpp
OBJ = main.o helper.o requests.o ip.o sql.o

%.o: %.c $(DEPS)
	$(CC) `mysql_config --cflags` -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) `mysql_config --libs`

clean:
	rm -rf $(OBJ)
