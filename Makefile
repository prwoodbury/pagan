IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

AR=ar
CXX=g++
CDEFINES=
CLIBS=
CFLAGS=-std=gnu++11 -O2 -Wall
CINCLUDES=
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR) $(CLIBS)

SRC := $(wildcard $(SRCDIR)/*/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%,$(ODIR)/%,$(patsubst %.cpp,%.o,$(SRC)))
DEP := $(patsubst %.o,%.d,$(OBJ))
LIB := libpagan.a

all: pagan
	
pagan: $(LIB)
	$(CXX) $(OBJ) src/main.cpp -o pagan $(CXXFLAGS)

error1: pagan
	cat Error1Example.pgn
	
error1x: pagan
	./pagan Error1Example.pgn

error2: pagan
	cat Error2Example.pgn
	
error2x: pagan
	./pagan Error2Example.pgn

error3: pagan
	cat Error3Example.pgn
	
error3x: pagan
	./pagan Error3Example.pgn

arrays: pagan
	cat ArraysExample.pgn
	
arraysx: pagan
	./pagan ArraysExample.pgn

conditionals: pagan
	cat ConditionalsExample.pgn
	
conditionalsx: pagan
	./pagan ConditionalsExample.pgn

recursion: pagan
	cat RecursionExample.pgn
	
recursionx: pagan
	./pagan RecursionExample.pgn

iteration: pagan
	cat IterationExample.pgn
	
iterationx: pagan
	./pagan IterationExample.pgn

functions: pagan
	cat FunctionsExample.pgn
	
functionsx: pagan
	./pagan FunctionsExample.pgn

dictionary: pagan
	cat DictionaryExample.pgn
	
dictionaryx: pagan
	./pagan DictionaryExample.pgn

problem: pagan
	echo "/*****Calculator program code*****/"
	cat RPNCalculator.pgn
	echo "/*****Input expression (modified from Wikipedia)*****/"
	echo "/*****Equivalent to (2 ^ (5 + ((1 + 2) * 4) - 3))*****/"
	cat testProblemInputFile
	
problemx: pagan
	cat testProblemInputFile | ./pagan RPNCalculator.pgn
	
$(LIB): $(OBJ)
	$(AR) rcs $@ $^

-include $(DEP)

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -MMD -MP -MF $(@:.o=.d) -o $@ $< $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(LIB)
	rm -rf $(ODIR)
	rm -f pagan
