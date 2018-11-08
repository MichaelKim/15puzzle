CFLAGS= -std=c++14 -Wall -Werror
SRC = src/*.cpp

puzzle: main.cpp $(SRC)
	g++ -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm puzzle
