OBJS = readWrite-bmp.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

readWrite-bmp : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o readWrite-bmp

readWrite-bmp.o : readWrite-bmp.cc
	$(CC) $(CFLAGS) readWrite-bmp.cc

clean:
	\rm -f *.o *~ readWrite-bmp

