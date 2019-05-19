CC          := g++ -O0 -g -Wall -std=c++14 -Wextra -pedantic 
INC         := -I../src
LIBS        := 
SRC         := $(wildcard *.cpp)
DEP         := $(wildcard ../src/*)

.PHONY: all clean

all : test.out
	@:

test.out : $(SRC) $(DEP)
	@/bin/rm -f $@
	$(CC) $(SRC) $(INC) $(LIBS) -o $@

clean : 
	@/bin/rm -rf *.o 
	@/bin/rm -rf *.d 
	/bin/rm -rf test.out
