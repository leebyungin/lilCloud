TARGET = lilCloud.exe

SYSTEM = ./system
UI = ./ui
WEB_SERVER = ./web_server
MESSAGE = ./message
HAL = ./hal

INCLUDES = -I$(SYSTEM) -I$(UI) -I$(WEB_SERVER) -I$(MESSAGE) -I$(HAL)

CC = gcc
CXXLIBS = -lpthread -lm -lrt
CXXFLAGS = $(INCLUDES) -g -O0 -std=c++14
CXX = g++

objects = main.o system_server.o web_server.o input.o gui.o message.o camera_HAL.o ControlThread.o

$(TARGET): $(objects)
	$(CXX) -o $(TARGET) -rdynamic $(objects)

main.o:  main.c
	$(CC) -c main.c -g $(INCLUDES)

system_server.o: $(SYSTEM)/system_server.h $(SYSTEM)/system_server.c
	$(CC) -c ./system/system_server.c -g $(INCLUDES) 

gui.o: $(UI)/gui.h $(UI)/gui.c
	$(CC) -c $(UI)/gui.c -g $(INCLUDES) 

input.o: $(UI)/input.h $(UI)/input.c
	$(CC) -c $(UI)/input.c -g $(INCLUDES) 

web_server.o: $(WEB_SERVER)/web_server.h $(WEB_SERVER)/web_server.c
	$(CC) -c $(WEB_SERVER)/web_server.c -g $(INCLUDES) 

message.o: $(MESSAGE)/message.h $(MESSAGE)/message.c
	$(CC) -c $(MESSAGE)/message.c -g $(INCLUDES)

camera_HAL.o: $(HAL)/camera_HAL.h $(HAL)/camera_HAL.cpp $(HAL)/ControlThread.cpp
	$(CXX) -c $(HAL)/camera_HAL.cpp $(CXXFLAGS) -g $(INCLUDES)

ControlThread.o: $(HAL)/ControlThread.h $(HAL)/ControlThread.cpp
	$(CXX) -c $(HAL)/ControlThread.cpp $(CXXFLAGS) $(objetcts) -g $(INCLUDES)

clean:
	rm -rf *.o
