CC=clang++
CFLAGS= -g -c -Wall --std=c++17 -I ../lib/
LDFLAGS= 
LIBS=
SOURCES=input_generator.cpp \
				../src/json.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

check: 
	clang-tidy -p . $(SOURCES)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clear:
	rm -f $(OBJECTS)
	rm -f main