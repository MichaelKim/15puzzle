#include "../include/Direction.h"

std::ostream& operator<<(std::ostream& out, const Direction& dir) {
    switch (dir) {
        case Direction::U:
            out << "D";
            break;
        case Direction::R:
            out << "L";
            break;
        case Direction::D:
            out << "U";
            break;
        default:
            out << "R";
            break;
    }
    return out;
}