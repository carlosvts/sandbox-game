# Executáveis
GAME_TARGET = sandbox
BADAPPLE_TARGET = badapple

# Compiler
CXX = g++

# Flags
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Raylib
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# --------------------
# Sandbox
# --------------------

GAME_SRCS = \
	src/main.cpp \
	src/game.cpp

GAME_OBJS = $(GAME_SRCS:.cpp=.o)

# --------------------
# Bad Apple Tool
# --------------------

BADAPPLE_SRCS = \
	tools/badapple.cpp

BADAPPLE_OBJS = $(BADAPPLE_SRCS:.cpp=.o)

# --------------------
# Build all
# --------------------

all: $(GAME_TARGET) $(BADAPPLE_TARGET)

# --------------------
# Sandbox
# --------------------

$(GAME_TARGET): $(GAME_OBJS)
	$(CXX) $(GAME_OBJS) -o $(GAME_TARGET) $(LDFLAGS)

# --------------------
# Bad Apple
# --------------------

$(BADAPPLE_TARGET): $(BADAPPLE_OBJS)
	$(CXX) $(BADAPPLE_OBJS) -o $(BADAPPLE_TARGET) $(LDFLAGS)

# --------------------
# Generic object rule
# --------------------

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --------------------
# Convenience targets
# --------------------

run: $(GAME_TARGET)
	./$(GAME_TARGET)

run-badapple: $(BADAPPLE_TARGET)
	./$(BADAPPLE_TARGET)

clean:
	rm -f \
		$(GAME_OBJS) \
		$(BADAPPLE_OBJS) \
		$(GAME_TARGET) \
		$(BADAPPLE_TARGET)

.PHONY: all clean run run-badapple
