OS := $(shell uname)
CC=g++

ifeq ($(OS),Linux)
	DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
	INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -Ilib -Iinclude -I.
	CXXFLAGS      = -Wno-missing-field-initializers -Wno-missing-braces -m64 -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES) $(INCPATH)
	LDFLAGS       = -pedantic -m64 -Wl,-O1
	LDLIBS        = -L/usr/lib/x86_64-linux-gnu -lQtGui -lQtCore -lpthread

	CFLAGS=-lsfml-network -lsfml-system -lssl -lcrypto -I. -Wall -L $(PWD)/lib -Wl,-rpath=$(PWD)/lib/ -lpal
	OBJ = src/main.o src/helper.o src/requests.o src/ip.o src/sql.o src/encode.o src/clipIt.o src/file_hash.o src/cpu_rec.o src/linux/kern.o src/linux/call.o src/linux/open.o
else
	CXXFLAGS = `wx-config --cxxflags`
	LDFLAGS  = `wx-config --ldflags`
	LDLIBS   = `wx-config --libs`

	CFLAGS= -Wall -Wl,-no_pie -lboost_system-mt -lboost_chrono-mt -lsfml-network -lsfml-system -lssl -lcrypto -I.
	OBJ = src/main.o src/helper.o src/requests.o src/ip.o src/sql.o src/encode.o src/clipIt.o src/file_hash.o src/cpu_rec.o src/mac/call.o src/mac/open.o
endif

DEPS = include/helper.hpp include/requests.hpp include/ip.hpp include/sql.hpp include/encode.hpp include/clipIt.hpp include/file_hash.hpp include/cpu_rec.hpp include/kern.hpp

%.o: %.c $(DEPS)
	$(CC) $(LDFLAGS) `mysql_config --cflags` -c $(CXXFLAGS) -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) `mysql_config --libs` $(LDLIBS)

clean:
	rm -rf $(OBJ)
