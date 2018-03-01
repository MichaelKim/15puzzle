#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include "Board.h"
#include "PartialDatabase.h"

#include <vector>

class DisjointDatabase {
    private:
        std::vector<PartialDatabase*> databases;
        int numDatabases;
        std::vector<int> where;
        std::vector<uint64_t> ids;

    public:
        DisjointDatabase(std::string name, std::vector<std::vector<std::vector<int>>> grids);
        virtual ~DisjointDatabase();

        int getHeuristic(const Board& board);
};

template <size_t N> struct uint_{ };
template <size_t N, typename Lambda, typename IterT>
inline void unroller(const Lambda& f, const IterT& iter, uint_<N>) {
    unroller(f, iter, uint_<N-1>());
    f(iter + N);
}

template <typename Lambda, typename IterT>
inline void unroller(const Lambda& f, const IterT& iter, uint_<0>) {
	 f(iter);
}

#endif // DISJOINTDATABASE_H
