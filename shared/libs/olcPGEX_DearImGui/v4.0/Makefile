SRCDIR = .
INC    = -I. -I../../olcPixelGameEngine/v2.27

CC     = emcc
CXX    = em++

CXXFLAGS = -O1 -std=c++20 $(INC)

# Find all cpp files and corresponding object files
SRCS = $(shell find $(SRCDIR) -name '*.cpp')
OBJS = $(SRCS:.cpp=.o)

# Target: build all objects
all: $(OBJS) imgui_impl_pge.a

# Recipe to build .o files from .cpp
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

imgui_impl_pge.a: $(OBJS)
	$(AR) rcs imgui_impl_pge.a $(OBJS)

# Clean up all generated object files
clean:
	@find $(SRCDIR) -name '*.o' -delete -exec echo {} \;
	@find $(SRCDIR) -name '*.a' -delete -exec echo {} \;


.PHONY: all clean