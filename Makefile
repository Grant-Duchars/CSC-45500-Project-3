# This is (at least the start of) a makefile for CSC45500, 
#   project#2
## NOTE: any LINE starting with a '#' in a Makefile is a comment

#OPTIM=-O
DBG=-g
OPTS=$(DBG) $(OPTIM) -Wall -ansi -pedantic -std=c++20

CC=g++ $(OPTS)  -c
LNK=g++

OBJS=Token.o Grammar.o MemManager.o main.o
GLBL=Makefile Token.hpp Grammar.hpp MemManager.hpp

project2: $(OBJS)
	$(LNK) -o project3 $(OBJS)

main.o: main.cpp Token.hpp Grammar.hpp MemManager.hpp $(GLBL)
	$(CC) main.cpp

Token.o: Token.cpp Token.hpp $(GLBL)
	$(CC) Token.cpp

Grammar.o: Grammar.cpp Grammar.hpp $(GLBL)
	$(CC) Grammar.cpp

MemManager.o: MemManager.cpp MemManager.hpp $(GLBL)
	$(CC) MemManager.cpp

clean:
	/bin/rm -f *.o project2 *~
