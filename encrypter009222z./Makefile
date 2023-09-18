# Compiler
CC = g++
# Compiler flags
FLAGS = -g -Wall -std=c++17

# Directories
HDR = src
SRC = src
OBJ = obj
TEST = test
# for generated files
GEN = gen

# Target name
TARGET = encrypter009222z.
TEST_TARGET = encrypter009222z._test

# All source files inside $(SRC) directory
SRCS = $(wildcard $(SRC)/*.cpp)
# All object files from those source files
OBJS = $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

# Object files to test target
SRC_TO_TEST = $(SRC)/huffman.cpp
OBJ_TO_TEST = $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRC_TO_TEST))

TEST_SRC = $(wildcard $(TEST)/*.cpp)
TEST_OBJ = $(patsubst $(TEST)/%.cpp, $(OBJ)/%.o, $(TEST_SRC))

# Main target to run with `make`
all: $(TARGET)

# Create directory for .o files
$(OBJ):
	mkdir $@
	
$(GEN):
	mkdir $@
	
# Compile all source files with $(HDR) directory included in search path
$(OBJ)/%.o: $(SRC)/%.cpp $(HDR)/%.h $(OBJ)
	$(CC) $(FLAGS) -c $< -o $@ -I $(HDR)
	
$(OBJ)/%.o: $(SRC)/%.cpp $(OBJ)
	$(CC) $(FLAGS) -c $< -o $@ -I $(HDR)

# With all the object files in place, compile the final target
$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@
	

# TEST TARGETS
$(OBJ)/%.o: $(TEST)/%.cpp $(TEST)/%.h $(OBJ)
	$(CC) $(FLAGS) -c $< -o $@ -I $(TEST) -I $(SRC)
	
$(OBJ)/%.o: $(TEST)/%.cpp $(OBJ)
	$(CC) $(FLAGS) -c $< -o $@ -I $(TEST) -I $(SRC)

$(TEST_TARGET): $(TEST_OBJ) $(OBJ_TO_TEST)
	$(CC) $(FLAGS) $^ -o $@
	
test: $(TEST_TARGET) $(GEN)
	
# Remove $(OBJ) directory and executable
clean:
	rm -fr $(OBJ) $(TARGET) $(TEST_TARGET) gen
