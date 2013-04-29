CC=gcc
CFLAGS=-c -g
LDFLAGS=-L/usr/local/lib -lgmp
SOURCES=test.c types/table.c types/hash_fn.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=table-test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $<

clean:
	rm -f *.o types/*.o *~ $(EXECUTABLE)
