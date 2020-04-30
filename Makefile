####################################################
##
## Makefile for CSCI Operating Systems Home Project
## Jonathan Kelaty
## LINUX Compilation
##
######################################################

# Compiler
CXX = g++
# Compilation flags
CXX_FLAGS = -g -std=c++17 -Wall
# Directory
EXEC_DIR = .

# Include object files for each program
main_INCLUDES = OS.o

# Source files to compile
SRCS = OS.cpp main.cpp

# Convert list of source files to list of object files
OBJECTS := $(patsubst %.cpp, %.o, $(SRCS))

# Programs
PROGRAMS = main

all:
	make $(PROGRAMS)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

$(PROGRAMS): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) -o $(EXEC_DIR)/$@ $@.o $($@_INCLUDES)

clean:
	rm -f *.o $(PROGRAMS)
