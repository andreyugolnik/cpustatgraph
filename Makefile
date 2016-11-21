CC=g++
CFLAGS=-O2 -c -Wall -std=c++11 -I/usr/include
LDFLAGS=-s -lxcb -lpthread
SOURCES=src/cpustats.cpp src/window.cpp src/main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=cpustatgraph

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f src/*.o
