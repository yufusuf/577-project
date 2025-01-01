# Compiler and flags
CC = mpicc
CFLAGS = -Wall -Wextra -g

# Libraries
LIBS = -llapacke -lblas -lm

# Source files
TEST_SRCS = test.c linear_solver.c cyclic.c tridiagonal_matrix.c

# Header files
HEADERS = linear_solver.h 

# Object files
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Targets
TEST_TARGET = test

# Default target
all: $(TEST_TARGET)

# Build the test executable
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(LIBS)

# Compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TEST_OBJS) $(TEST_TARGET)
run_test:
	./test

# PHONY targets
.PHONY: all clean

