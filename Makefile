-include Makefile.config

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
BIN = bin/linuxdiag

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

install:
	@mkdir -p $(PREFIX)/bin
	cp $(BIN) $(PREFIX)/bin/linuxdiag
	echo "Installed to $(PREFIX)/bin/linuxdiag"

clean:
	rm -rf build/ bin/