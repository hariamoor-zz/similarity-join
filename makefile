
COMPILER = g++
CCFLAGS = -O3 -std=c++11
METHOD = SimilarityJoin

all: $(METHOD)

SimilarityJoin: main.o simJoin.o
	${COMPILER} ${CCFLAGS} -o $@ main.o simJoin.o

main.o: main.cc simJoin.o
	${COMPILER} ${CCFLAGS} -c -o $@ $<

simJoin.o: simJoin.cc simJoin.h
	${COMPILER} ${CCFLAGS} -c -o $@ $<

.PHONY: clean
clean :
	rm *.o SimilarityJoin
