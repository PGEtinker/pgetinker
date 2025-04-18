SRCDIR = .
INC    = -IolcPixelGameEngine -IolcPixelGameEngine/extensions -IolcPixelGameEngine/utilities -Iminiaudio/0.11.21

CC     = emcc
CXX    = em++

CXXFLAGS = -std=c++20 -c $(INC)

# Find all cpp files and corresponding object files
SRCS = $(shell find $(SRCDIR) -name '*.cpp')
OBJS = $(SRCS:.cpp=.o)

# Target: build all objects
all: $(OBJS)

# Recipe to build .o files from .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up all generated object files
clean:
	@find $(SRCDIR) -name '*.o' -delete -exec echo {} \;


.PHONY: all clean