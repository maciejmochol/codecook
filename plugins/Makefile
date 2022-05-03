CC=g++
CFLAGS=-g -Wall -D_REENTRANT -I../src/
LIBPTHREAD=-lpthread
OBJ= system_info.o praterm.o
#OBJ= system_info.o

all: $(OBJ) ;

.cc.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core *~ 
