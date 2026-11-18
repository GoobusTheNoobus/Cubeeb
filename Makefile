# Makefile commands:
# make  				- builds a release binary in `build/cubeeb`
# make release			- builds a release binary in `build/cubeeb`
# make debug			- builds a debug binary in `build-debug/cubeeb`
# make clean 			- nukes all build directories

# Set some variables
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra

SRC := $(wildcard src/*.cpp)

# Debug object files would be in build-debug/ while release files would be in build/
DEBUG_OBJ := $(patsubst src/%.cpp,build-debug/%.o,$(SRC))
RELEASE_OBJ := $(patsubst src/%.cpp,build/%.o,$(SRC))

.PHONY: all debug release clean

# By default, it builds release
all: release

debug: build-debug/cubeeb
release: build/cubeeb

# Links binaries

build-debug/cubeeb: $(DEBUG_OBJ)
	$(CXX) $^ -o $@

build/cubeeb: $(RELEASE_OBJ)
	$(CXX) $^ -o $@

# Compiles source files to objects 

build-debug/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -O0 -g -c $< -o $@

build/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -O3 -c $< -o $@

# Nukes build files

clean:
	rm -rf build build-debug
