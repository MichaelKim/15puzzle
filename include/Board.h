#ifndef BOARD_H
#define BOARD_H

#include "DisjointDatabase.h"
#include "Point.h"

#include <ostream>
#include <vector>

class Board {
public:
    enum class Move : int { Null, N, E, W, S };

    class State {
    public:
        uint64_t getID() const {
            return grid;
        }
        Point getBlank() {
            return blank;
        }
        int getHeuristic() {
            return h;
        }
        uint64_t grid;
        Point blank;
        int h;
    };

private:
    static const std::vector<std::vector<Move>> moves;

    DisjointDatabase* db;

    inline void setCell(State& state, int x, int y, int n);
    inline int getCell(const State& state, int x, int y) const;

public:
    static const int MAX_LENGTH = 16;
    const int WIDTH, HEIGHT;

    Board(DisjointDatabase* db, int w, int h);
    virtual ~Board();

    State init(std::vector<std::vector<int>> g);
    const std::vector<Move>& getMoves(const State& state, Move prevMove);
    int applyMove(State& state, Move dir);
    void undoMove(State& state, Move dir);

    friend std::ostream& operator<<(std::ostream& out, const Move& move);
    friend std::ostream& operator<<(std::ostream& out, const State& state);
};

#endif  // BOARD_H
