
CC := gcc

SRCS := sniff.c util.c arp.c route.c

defalut : $(SRCS)
	$(CC) -c $(SRCS)
	ar rc -o libsniff.a sniff.o
	ar rc -o libutil.a util.o
	ar rc -o libarp.a arp.o
	ar rc -o libroute.a route.o

