
CC := gcc

SRCS := sniff.c util.c

defalut : $(SRCS)
	$(CC) -c $(SRCS)
	ar rc -o libsniff.a sniff.o
	ar rc -o libutil.a util.o	
