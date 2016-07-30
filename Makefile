CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-type-limits -g3 -O0 -Wpointer-arith -fvisibility=hidden

LIBS = -lpthread
SOURCES = main.c matrix.c matrix_thread.c queue.c tqueue.c tpool.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE = matrix_multiply mm 

#.o : .c
#	$(CC) $(CFLAGS) –c -o $@ $<

#%.o : %.c
#	$(CC) $(CFLAGS) –c -o $@ $<

all: $(EXECUTABLE)     

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

clean:
	rm -f *.o $(EXECUTABLE)
