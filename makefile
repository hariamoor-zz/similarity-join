COMPILER = g++
CCFLAGS = -O3 -std=c++11
METHOD = SimilarityJoin
TEST = SimilarityJoinTest

all: $(METHOD)

test: simJoinTest.o
	$(COMPILER) -o $@ simJoinTest.o simJoin.o

simJoinTest.o: simJoinTest.cc simJoin.o
	${COMPILER} -c -o $@ $<

SimilarityJoin: main.o simJoin.o
	${COMPILER} ${CCFLAGS} -o $@ main.o simJoin.o

main.o: main.cc simJoin.o
	${COMPILER} ${CCFLAGS} -c -o $@ $<

simJoin.o: simJoin.cc simJoin.h
	${COMPILER} ${CCFLAGS} -c -o $@ $<

.PHONY: clean
clean :
	rm -f *.o SimilarityJoin $(TEST)
