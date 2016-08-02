CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-type-limits -g3 -O3 -Wpointer-arith -fvisibility=hidden

LIBS = -lpthread
SOURCES = main.c matrix.c matrix_thread.c queue.c tqueue.c tpool.c tevent.c ttask.c
OBJECTS=$(SOURCES:.c=.o)

test_SOURCES = ./tests/matrix_suite.c
test_OBJECTS = $(test_SOURCES:.c=.o)
test_INCLUDE = -I./

EXECUTABLE = matrix_multiply mm 

TESTS = matrix_suite

$(test_OBJECTS) : $(test_SOURCES)
	$(CC) -c $(CFLAGS) $(test_INCLUDE) -o $@ $<

#.o : .c $(HEADERS)
#	$(CC) $(CFLAGS) –c -o $@ $<

#%.o : %.c
#	$(CC) -c $(CFLAGS) -o $@ $<

all: $(EXECUTABLE)
tests: $(TESTS)     
$(TESTS): $(test_OBJECTS) 
	$(CC) $(LDFLAGS) $(test_OBJECTS) -o $@

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

clean:
	rm -f *.o $(EXECUTABLE)
