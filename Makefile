CC=clang++
CFLAGS=-g -c -Wall -Wextra --std=c++17 -I lib/ -I tests/lib/
LDFLAGS=
LIBS= 
SOURCES=main.cpp \
        src/json_reader.cpp \
        src/json.cpp \
        src/map_renderer.cpp \
        src/request_handler.cpp \
        src/svg.cpp \
        src/transport_catalogue.cpp \
        tests/src/tests_transport.cpp \
				src/geo.cpp \
				src/json_builder.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clear:
	rm -f $(OBJECTS)
	rm -f main