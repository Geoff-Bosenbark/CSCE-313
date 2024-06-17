# @file makefile
# @brief Makefile for building the Load Balancer project.
# @details This Makefile compiles and links the C++ source files for the Load Balancer project.
# It also provides a clean target to remove generated files.

# Compiler
CC = g++

# Compiler Flags
CFLAGS = -Wall -Werror

# Target Executable
TARGET = loadbalancer

# Source Files
SRCS = main.cpp LoadBalancer.cpp Request.cpp WebServer.cpp

# Object Files
OBJS = $(SRCS:.cpp=.o)

# Default Target
# @brief Default target to build the executable.
all: $(TARGET)

# Link the object files to create the executable
# @brief Link the object files to create the executable.
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
# @brief Compile source files into object files.
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
# @brief Clean up build files.
clean:
	rm -f $(TARGET) $(OBJS)