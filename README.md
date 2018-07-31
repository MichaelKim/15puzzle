# 15puzzle

This is a solver for the generalized 15-puzzle. It can solve any m x n sized board, although it becomes very slow for boards larger than 4 x 4, and some 4 x 4 boards with long solutions.

To solve a puzzle, it uses the IDA\* algorithm with an [additive disjoint pattern database](https://www.sciencedirect.com/science/article/pii/S0004370201000923).

## Build

```
g++ -std=c++14 main.cpp src/*.cpp -o puzzle
```

## Usage

```
Syntax:
  puzzle [OPTIONS]

Options:
  -b <file>
    Board files
  -d <file>
    Use database file
  -h, --help
    Print this help
  -i, --interactive
    Show a playback of each solution
  -p, --parallel
    Run multithreaded IDA* (experimental)
```

Alternatively, the database and board files can be read in through standard input.

```
cat databases/8-reg boards/8-reg | ./puzzle
```

## Databases

A database file consists of three parts:

- The first line contains integers `p` and `q`, which represent the width and height of the board to be solved
- The second line has the number of patterns in the databases, `n`
- The `n` patterns follow:
  - Each pattern is a `p` x `q` board filled with numbers. Each tile that is part of the pattern has its number in its position, and the remaining irrelevant tiles have a 0.

This program comes with several preset board sizes and database patterns. However, it is very simple to create custom boards and databases to solve any sized boards.

- `databases/8-reg` uses only one pattern for the entire 3x3 grid, as it is small enough to quickly generate the database.
- `databases/443-reg` is for solving the 11-puzzle.
- `databases/555-reg` and `databases/663-reg` both solve the 15-puzzle. The 663 pattern takes longer to generate (as larger patterns take exponentially longer), but solve it slightly faster than the 555 pattern.
- `databases/555-rev` is for the reversed 15-puzzle board.

There are two versions of the solved state of the 15-puzzle: one has the empty tile in the top-left, and the other has it in the bottom-right. These two states have different parity, so one cannot be turned into the other. Hence, `555-rev` solves the reversed board with the empty tile in the top-left, while `555-reg` has the empty tile in the bottom-right.

## Results

All 8-puzzle boards can be solve in a fraction of a second. The hardest 8-puzzle boards take 31 moves to solve, and even those are solved instantly.

11-puzzle (4x3) boards can be solved relatively quickly, but I haven't tested all of them thoroughly.

Most 15-puzzle can be solved within 5 seconds. Solutions longer than ~55 moves will take significantly longer.
