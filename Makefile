CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -g

puzzle: obj/main.o obj/Board.o obj/Direction.o obj/DisjointDatabase.o obj/Idastar.o obj/InputParser.o obj/PartialDatabase.o obj/Pattern.o | bin
	$(CXX) $(CXXFLAGS) -o bin/puzzle $^

obj/main.o: src/main.cpp include/Board.h include/DisjointDatabase.h include/Idastar.h include/InputParser.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Board.o: src/Board.cpp include/Board.h include/DisjointDatabase.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Direction.o: src/Direction.cpp include/Direction.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/DisjointDatabase.o: src/DisjointDatabase.cpp include/DisjointDatabase.h include/Board.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Idastar.o: src/Idastar.cpp include/Idastar.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/InputParser.o: src/InputParser.cpp include/InputParser.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/PartialDatabase.o: src/PartialDatabase.cpp include/PartialDatabase.h include/Pattern.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Pattern.o: src/Pattern.cpp include/Pattern.h include/Direction.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin:
	mkdir -p $@
obj:
	mkdir -p $@

.PHONY: clean
clean:
	rm -r bin/ obj/ databases/*.dat
