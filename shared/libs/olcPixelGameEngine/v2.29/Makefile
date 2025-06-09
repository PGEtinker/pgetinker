SRCDIR = .
INC    = -I. -I./extensions -I./utilities

CC     = emcc
CXX    = em++

CXXFLAGS = -O1 -std=c++20 $(INC)

# Find all cpp files and corresponding object files
SRCS = $(shell find $(SRCDIR) -name '*.cpp')
OBJS = $(SRCS:.cpp=.o)

# Target: build all objects
all: $(OBJS)

# Recipe to build .o files from .cpp
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

# Clean up all generated object files
clean:
	@find $(SRCDIR) -name '*.o' -delete -exec echo {} \;


.PHONY: all clean