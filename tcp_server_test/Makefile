CXX = gcc
LDFLAGS = -lstdc++ -pthread
FLAGS = -std=c++14 -Wall

INC = `find inc -name \*.cpp -o -name \*.hpp`
SRC = `find src -name \*.cpp -o -name \*.hpp`

build:
	mkdir -p bin
	$(CXX) $(FLAGS) $(LDFLAGS) $(INC) $(SRC) -o bin/server

clean:
	rm -rf bin
