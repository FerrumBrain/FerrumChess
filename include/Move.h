#pragma once

struct Cell {
    int x = -1, y = -1;

    Cell() = default;
    Cell(int x_, int y_) : x(x_), y(y_) {}
};

struct Move {
    Cell from, to;

    Move() = default;
    Move(Cell a, Cell b) : from(a), to(b) {}
};

inline bool operator==(const Cell &a, const Cell &b) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator==(const Move &a, const Move &b) {
    return a.from == b.from && a.to == b.to;
}