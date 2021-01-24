CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -g3

all: BlottoGA Unit

BlottoGA: strategy.o population.o blotto_ga.o arrays.o blotto.o ga_helpers.o
	${CC} ${CFLAGS} -o $@ $^ 

Unit: ga_unit.o blotto.o ga_helpers.o population.o strategy.o arrays.o
	${CC} ${CFLAGS} -o $@ $^


strategy.o: strategy.h blotto.h arrays.h
blotto_ga.o: strategy.h population.h ga_helpers.h arrays.h
blotto.o: blotto.h 
ga_helpers.o: ga_helpers.h strategy.h population.h 
population.o: population.h strategy.h
ga_unit.o: strategy.h population.h ga_helpers.h
arrays.o: arrays.h

