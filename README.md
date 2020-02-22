# 15puzzle

This is a solver for the generalized 15-puzzle written in C++. It can optimally solve any p x q sized board, although it becomes very slow for boards larger than 4 x 4, and some 4 x 4 boards with long solutions.

To solve a puzzle, it uses the IDA\* algorithm with an [additive disjoint pattern database](https://www.sciencedirect.com/science/article/pii/S0004370201000923).

This solver has also been ported to WebAssembly using Emscripten [here](https://github.com/LenKagamine/15puzzle.js).

## Executable

### Build

```sh
make

# or

mkdir build
cd build
cmake ..

# or

g++ -std=c++17 src/*.cpp -o bin/puzzle
```

### Usage

```
Syntax:
  puzzle [OPTIONS]

Options:
  -b <file>
    Board files
  -d <file>
    Use database file
  -h, --help
    Print this help message
  -i, --interactive
    Show a playback of each solution
```

Alternatively, the database and board files can be read in through standard input. The following three commands will do the same solves:

```sh
cat databases/8-reg | ./puzzle -b boards/8-reg
cat boards/8-reg | ./puzzle -d databases/8-reg
cat databases/8-reg boards/8-reg | ./puzzle
```

## File Formats

This program comes with several preset board sizes and database patterns. However, it is very simple to create custom boards and databases to solve any sized boards.

- `databases/8-reg` uses only one pattern for the entire 3x3 grid, as it is small enough to quickly generate the database.
- `databases/443-reg` is for solving the 11-puzzle.
- `databases/555-reg` and `databases/663-reg` both solve the 15-puzzle. The 663 pattern takes longer to generate (as larger patterns take exponentially longer), but solve it slightly faster than the 555 pattern.
- `databases/555-rev` is for the reversed 15-puzzle board.

There are two versions of the solved state of the 15-puzzle: one has the empty tile in the top-left, and the other has it in the bottom-right. These two states have different parity, so one cannot be turned into the other. Hence, `555-rev` solves the reversed board with the empty tile in the top-left, while `555-reg` has the empty tile in the bottom-right.

### Databases

A database file consists of three parts:

- `p` and `q`: The width and height of the boards that this database solves
- `n`: The number of patterns in the databases
- The `n` patterns follow:
  - Each pattern is a `p` x `q` board filled with numbers. Each tile that is part of the pattern has its number in its position, and the remaining irrelevant tiles have a 0.

### Boards

A board file consists of three parts:

- `p` and `q`: The width and height of the following boards to be solved
- `n`: The number of boards to solve
- The `n` boards follow:
  - Each board is a permutation of `p x q` numbers from 0 to `p x q - 1`. These numbers correspond to the tiles of the board, left-to-right and top-to-down. The blank is represented with a 0.

## Results

All 8-puzzle boards can be solve in a fraction of a second. The hardest 8-puzzle boards take 31 moves to solve, and even those are solved instantly.

11-puzzle (4x3) boards can be solved relatively quickly, but I haven't tested all of them thoroughly.

Most 15-puzzle can be solved within 5 seconds. Solutions longer than ~55 moves will take significantly longer.

Any larger boards (5x4, 5x5, etc.) with non-trivial solutions will take extremely long to solve.
