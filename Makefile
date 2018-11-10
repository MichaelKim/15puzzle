CXX = g++
CXXFLAGS = -std=c++14 -Wall -Werror
export EMCC_DEBUG=1

puzzle: obj/main.o obj/Board.o obj/DisjointDatabase.o obj/InputParser.o obj/PartialDatabase.o obj/Pattern.o
	$(CXX) $(CXXFLAGS) -o bin/puzzle $^

obj/main.o: src/main.cpp include/Board.h include/DisjointDatabase.h include/Idastar.h include/Idastar-inl.h include/ctpl_stl.h include/InputParser.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Board.o: src/Board.cpp include/Board.h include/Point.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/DisjointDatabase.o: src/DisjointDatabase.cpp include/DisjointDatabase.h include/Board.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/InputParser.o: src/InputParser.cpp include/InputParser.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/PartialDatabase.o: src/PartialDatabase.cpp include/PartialDatabase.h include/Board.h include/Pattern.h include/flat_hash_map.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
obj/Pattern.o: src/Pattern.cpp include/Pattern.h include/Direction.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<


web: wobj/wasm.o wobj/Board.o wobj/DisjointDatabase.o wobj/PartialDatabase.o wobj/Pattern.o
	em++ --bind --std=c++14 -o wbin/puzzle.js $^ \
		-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 \
		-s "EXTRA_EXPORTED_RUNTIME_METHODS=['addOnPostRun']"

wobj/wasm.o: web/wasm.cpp include/Board.h include/DisjointDatabase.h web/Idastar.h web/Idastar-inl.h
	em++ $(CXXFLAGS) -c -o $@ $<
wobj/Board.o: src/Board.cpp include/Board.h include/Point.h
	em++ $(CXXFLAGS) -c -o $@ $<
wobj/DisjointDatabase.o: src/DisjointDatabase.cpp include/DisjointDatabase.h include/Board.h
	em++ $(CXXFLAGS) -c -o $@ $<
wobj/PartialDatabase.o: src/PartialDatabase.cpp include/PartialDatabase.h include/Board.h include/Pattern.h include/flat_hash_map.h
	em++ $(CXXFLAGS) -c -o $@ $<
wobj/Pattern.o: src/Pattern.cpp include/Pattern.h include/Direction.h
	em++ $(CXXFLAGS) -c -o $@ $<


.PHONY: clean
clean:
	rm bin/puzzle obj/*.o wbin/puzzle.js wbin/puzzle.wasm wobj/*.o
