CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-type-limits -g3 -O3 -Wpointer-arith -fvisibility=hidden

SOURCES = main.c matrix.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE = matrix_multiply mm 

#.o : .c
#	$(CC) $(CFLAGS) –c -o $@ $<

#%.o : %.c
#	$(CC) $(CFLAGS) –c -o $@ $<

all: $(EXECUTABLE)     

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f *.o $(EXECUTABLE)
