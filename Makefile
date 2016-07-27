CC = gcc
SOURCES = main.c
CFLAGS = -Wall -Wextra -Wshadow -Wno-type-limits -g3 -O0 -Wpointer-arith -fvisibility=hidden
OBJECTS=$(SOURCES:.c=.o)

#$(SOURCES: %.cpp = %.o) - changes all .c files from SOURCES to .o
EXECUTABLE = matrix_multiply mm 

all: $(EXECUTABLE)     
obj: $(OBJECTS)     

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

#.o : .c
#	$(CC) $(CFLAGS) –c $(SOURCE) -o $@

#%.o : %.c
#	$(CC) $(CFLAGS) –c -o $@ $<

#.c.o:
#	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -f *.o $(EXECUTABLE)


