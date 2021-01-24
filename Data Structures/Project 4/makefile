CC = gcc
CFLAGS = -Wall -pendantic -std=c99 -g3

Rank: rank.o gmap.o ldigraph.o heapsort.o hash_key.o
	${CC} -o $@ ${CFLAGS} $^

rank.o: gmap.h ldigraph.h
ldigraph.o: ldigraph.h heapsort.h gmap.h hash_key.h
gmap.o: gmap.h
hash_hey.o: hash_key.h
heapsort.o: heapsort.h

