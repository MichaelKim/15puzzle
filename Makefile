CXX = g++
CXXFLAGS = -std=c++14 -Wall -Werror

puzzle: obj/main.o obj/Board.o obj/DisjointDatabase.o obj/InputParser.o obj/PartialDatabase.o obj/Pattern.o | bin
	$(CXX) $(CXXFLAGS) -o bin/puzzle $^

obj/main.o: src/main.cpp include/Board.h include/DisjointDatabase.h include/Idastar.h include/Idastar-inl.h include/ctpl_stl.h include/InputParser.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Board.o: src/Board.cpp include/Board.h include/Point.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/DisjointDatabase.o: src/DisjointDatabase.cpp include/DisjointDatabase.h include/Board.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/InputParser.o: src/InputParser.cpp include/InputParser.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/PartialDatabase.o: src/PartialDatabase.cpp include/PartialDatabase.h include/Board.h include/Pattern.h include/flat_hash_map.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Pattern.o: src/Pattern.cpp include/Pattern.h include/Direction.h | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin:
	mkdir -p $@
obj:
	mkdir -p $@

.PHONY: clean
clean:
	rm -r bin/ obj/
