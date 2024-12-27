# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g 

# Libraries
LIBS = -llapacke -lblas -lm

# Source files
MAIN_SRCS = main.c linear_solver.c cyclic.c
TEST_SRCS = test.c linear_solver.c cyclic.c

# Header files
HEADERS = linear_solver.h 

# Object files
MAIN_OBJS = $(MAIN_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Targets
MAIN_TARGET = main
TEST_TARGET = test

# Default target
all: $(MAIN_TARGET) $(TEST_TARGET)

# Build the main executable
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJS) $(LIBS)

# Build the test executable
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(LIBS)

# Compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(MAIN_OBJS) $(TEST_OBJS) $(MAIN_TARGET) $(TEST_TARGET)

# PHONY targets
.PHONY: all clean

