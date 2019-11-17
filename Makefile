CPP=clang++
CPPFLAGS=-g -Wall -std=c++11 -Iinc -Ilib

SRC=$(wildcard src/*.cpp) $(wildcard src/parser/*.cpp) $(wildcard src/translator/*.cpp) $(wildcard src/x86/*.cpp)
HDR=$(wildcard inc/*.hpp) $(wildcard inc/parser/*.hpp) $(wildcard inc/translator/*.hpp) $(wildcard inc/x86/*.hpp)
OBJ=$(SRC:src/%.cpp=build/%.o)

TEST_SRC=$(wildcard test/*.cpp)
TEST_OBJ=$(TEST_SRC:test/%.cpp=build/test/%.o) $(filter-out build/main.o, $(OBJ))


bin/c0c: $(OBJ) $(HDR)
	$(CPP) $(CPPFLAGS) $(OBJ) -o $@

build/x86/%.o: src/x86/%.cpp
	mkdir -p build/x86
	$(CPP) $(CPPFLAGS) -c $< -o $@

build/translator/%.o: src/translator/%.cpp
	mkdir -p build/translator
	$(CPP) $(CPPFLAGS) -c $< -o $@

build/parser/%.o: src/parser/%.cpp
	mkdir -p build/parser
	$(CPP) $(CPPFLAGS) -c $< -o $@

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
	rm -f bin/c0c
	rm -f bin/test
