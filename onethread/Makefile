CC = gcc
CFLAGS =-W -Wall -g -ggdb
CLIENTOBJ = network.o client.o
ONETHROBJ = network.o onethread.o
MULTIOBJ  = memutil.o socket.o sockbuf.o multiserv.o
UNBUFOBJ = memutil.o socket.o sockbuf.o telnet.o unbuf.o
OBJS = $(CLIENTOBJ) $(ONETHROBJ) $(MULTIOBJ)
SRCS = memutil.c network.c socket.c sockbuf.c telnet.c \
			 client.c onethread.c multiserv.c unbuf.c
PROG = client onethread multiserv unbuf
LIBS = 

all: $(PROG)

client: $(CLIENTOBJ)
	$(CC) $(CFLAGS) -o $@ $(CLIENTOBJ) $(LIBS)

onethread: $(ONETHROBJ)
	$(CC) $(CFLAGS) -o $@ $(ONETHROBJ) $(LIBS)

multiserv: $(MULTIOBJ)
	$(CC) $(CFLAGS) -o $@ $(MULTIOBJ) $(LIBS)

unbuf: $(UNBUFOBJ)
	$(CC) $(CFLAGS) -o $@ $(UNBUFOBJ) $(LIBS)

clean:
	rm -f *.o $(PROG)

.c.o:
	$(CC) $(CFLAGS) -c $<
