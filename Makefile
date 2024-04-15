TARGET = NAS.exe

SYSTEM = ./system
UI = ./ui
WEB_SERVER = ./web_server

INCLUDES = -I$(SYSTEM) -I$(UI) -I$(WEB_SERVER)

CC = gcc

objects = main.o system_server.o web_server.o input.o gui.o

$(TARGET): $(objects)
	$(CC) -o $(TARGET) $(objects)

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

clean:
	rm -rf *.o
