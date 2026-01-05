# Nome do executável final
TARGET = sandbox

# Compiler
CXX = g++

# Flags 
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

# source
SRCS = main.cpp game.cpp

# objects
OBJS = $(SRCS:.cpp=.o)

# Flags  Raylib
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# compile
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# .cpp into .o
%.o: %.cpp game.hpp utils.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# clear binary
clean:
	rm -f $(OBJS) $(TARGET)

# compile and run
run: $(TARGET)
	./$(TARGET)

.PHONY: clean run