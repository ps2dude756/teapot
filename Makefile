COMPILER = g++
COMPILER_FLAGS = -c -O0 -Wall
LINKER = g++
LINKER_FLAGS = -lGL -lGLU -lglut -lGLEW

all : teapot

teapot : teapot.o vectorfunctions.o readppm.o
	$(LINKER) $(LINKER_FLAGS) teapot.o vectorfunctions.o readppm.o -o teapot

teapot.o : teapot.cpp vectorfunctions.h readppm.h
	$(COMPILER) $(COMPILER_FLAGS) teapot.cpp

vectorfunctions.o : vectorfunctions.h vectorfunctions.cpp
	$(COMPILER) $(COMPILER_FLAGS) vectorfunctions.cpp

readppm.o : readppm.h readppm.cpp
	$(COMPILER) $(COMPILER_FLAGS) readppm.cpp

.PHONY : clean
clean :
	rm -f *.o teapot
