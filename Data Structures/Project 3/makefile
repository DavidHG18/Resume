CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -g3

all: GmapUnit CooccurUnit Cooccur

GmapUnit: gmap.o gmap_unit.o gmap_test_functions.o string_key.o
	${CC} -o $@ ${CFLAGS} $^

CooccurUnit: gmap_test_functions.o cooccur_unit.o gmap.o cooccur.o string_key.o
	${CC} -o $@ ${CFLAGS} $^

Cooccur: cooccur.o cooccur_main.o gmap.o string_key.o 
	${CC} -o $@ ${CFLAGS} $^


cooccur_unit.o: gmap_test_functions.h cooccur.h
gmap.o: gmap.h
cooccur.o: cooccur.h gmap.h
cooccur_unit.o: cooccur.h gmap_test_functions.h 
gmap_unit.o: gmap.h gmap_test_functions.h string_key.h
gmap_test_functions.o: gmap_test_functions.h gmap.h
string_key.o: string_key.h