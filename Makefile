CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-type-limits -g3 -O3 -Wpointer-arith -fvisibility=hidden

LIBS = -lpthread
SOURCES = main.c matrix.c matrix_thread.c
OBJECTS=$(SOURCES:.c=.o)

test_SOURCES = ./tests/matrix_suite.c
test_OBJECTS = $(test_SOURCES:.c=.o)

lib_SOURCES = lib/queue.c lib/tqueue.c lib/tpool.c lib/ttask.c lib/tevent.c
lib_OBJECTS = $(lib_SOURCES:.c=.o)

lib_INCLUDE = -I./lib

EXECUTABLE = matrix_multiply mm 

TESTS = matrix_suite

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(lib_OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(lib_OBJECTS)  -o $@ $(LIBS)


#This is taken when make all
%.o : %.c
	$(CC) -c $(CFLAGS) $(lib_INCLUDE) -o $@ $<

tests: $(TESTS)

$(TESTS): $(test_OBJECTS) 
	$(CC) $(LDFLAGS) $(test_OBJECTS) -o $@

$(test_OBJECTS) : $(test_SOURCES)
	$(CC) -c $(CFLAGS) $(lib_INCLUDE) -o $@ $<


clean:
	rm -f $(OBJECTS) \
	      $(lib_OBJECTS) \
	      $(test_OBJECTS) \
	      $(TESTS) \
	      $(EXECUTABLE)

#.o : .c $(HEADERS)
#	$(CC) $(CFLAGS) –c -o $@ $<

