CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Isrc/include

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
BIN = bin/linuxdiag

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^ -lncurses

# Compile .cpp into build/*.o
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build/ bin/
