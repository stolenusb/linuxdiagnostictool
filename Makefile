CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Isrc/include

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
BIN = bin/linuxdiag

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^ -lncurses

clean:
	rm -rf src/*.o bin/
