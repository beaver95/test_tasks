CXX = gcc
LDFLAGS = -lstdc++ -pthread
FLAGS = -std=c++14

INC = `find inc -name \*.cpp -o -name \*.hpp`
SRC = `find src -name \*.cpp -o -name \*.hpp`

build:
	mkdir -p bin
	$(CXX) $(FLAGS) $(LDFLAGS) $(INC) $(SRC) -o bin/sql_ps

clean:
	rm -rf bin
