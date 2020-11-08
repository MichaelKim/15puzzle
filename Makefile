CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -g

puzzle: obj/main.o obj/Board.o obj/BoardRect.o obj/Direction.o obj/DisjointDatabase.o obj/Idastar.o obj/InputParser.o obj/Pattern.o obj/WalkingDistance.o obj/Util.cpp | bin
	$(CXX) $(CXXFLAGS) -o bin/puzzle $^

obj/main.o: src/main.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Board.o: src/Board.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/BoardRect.o: src/BoardRect.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Direction.o: src/Direction.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/DisjointDatabase.o: src/DisjointDatabase.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Idastar.o: src/Idastar.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/InputParser.o: src/InputParser.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Pattern.o: src/Pattern.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/WalkingDistance.o: src/WalkingDistance.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Util.o: src/Util.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin:
	mkdir -p $@
obj:
	mkdir -p $@

.PHONY: clean
clean:
	rm -r bin/ obj/ databases/*.dat
