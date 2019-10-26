CPP=clang++
CPPFLAGS=-g -Wall -std=c++11 -Iinc -Ilib

SRC=$(wildcard src/*.cpp)
HDR=$(wildcard inc/*.hpp)
OBJ=$(SRC:src/%.cpp=build/%.o)

TEST_SRC=$(wildcard test/*.cpp)
TEST_OBJ=$(TEST_SRC:test/%.cpp=build/test/%.o) $(filter-out build/main.o, $(OBJ))


bin/c0c: $(OBJ) $(HDR)
	$(CPP) $(CPPFLAGS) $(OBJ) -o $@


build/%.o: src/%.cpp
	mkdir -p build
	$(CPP) $(CPPFLAGS) -c $< -o $@


test: $(TEST_OBJ) $(HDR)
	$(CPP) $(CPPFLAGS) $(TEST_OBJ) -o bin/$@
	bin/test

build/test/%.o: test/%.cpp
	mkdir -p build/test
	$(CPP) $(CPPFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -rf *.dSYM
	rm -rf build
