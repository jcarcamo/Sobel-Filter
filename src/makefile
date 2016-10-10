OBJS = readWrite-bmp.o
OBJS2 = readWrite-bmp-pthreads.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG) -pthread -std=c++11 
LFLAGS = -Wall $(DEBUG) -lpthread -lgomp

non-threaded :
	make readWrite-bmp.o
	make readWrite-bmp 

threaded :
	make readWrite-bmp-pthreads.o
	make readWrite-bmp-pthreads

readWrite-bmp-pthreads : $(OBJS2)
	$(CC) $(OBJS2) $(LFLAGS)  -o readWrite-bmp-threaded

readWrite-bmp : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o readWrite-bmp

readWrite-bmp.o : readWrite-bmp.cc
	$(CC) $(CFLAGS) readWrite-bmp.cc

readWrite-bmp-pthreads.o : readWrite-bmp-pthreads.cc
	$(CC) $(CFLAGS)  readWrite-bmp-pthreads.cc

all :
	make non-threaded
	make threaded

clean:
	\rm -f *.o *~ readWrite-bmp readWrite-bmp-threaded 

