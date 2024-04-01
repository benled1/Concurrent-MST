# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Source files directory
SRC_DIR = .
UTILS_DIR = utils

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
SRCS += $(wildcard $(UTILS_DIR)/*.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = main

# Main target
all: $(EXEC)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files into executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Clean
clean:
	rm -f $(EXEC) $(OBJS)
