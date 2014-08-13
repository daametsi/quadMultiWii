#all: 
#	g++ control.cpp dumpData.cpp itg3200.cpp PruProxy.cpp -o control

CC=g++
CFLAGS=-c -w
#CFLAGS=
LDFLAGS= 
SOURCES=control.cpp dumpData.cpp sensor.cpp itg4200.cpp PruProxy.cpp misc.cpp config.cpp adxl345.cpp gpio.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=control

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS)  -lrt -lpthread -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	