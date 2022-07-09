#pragma once

struct Cell {
    int x = -1;
    int y = -1;

    Cell() = default;

    Cell(int x_, int y_) : x(x_), y(y_) {}

    bool operator==(const Cell &b) const = default;
};

struct Move {
    Cell from;
    Cell to;

    Move() = default;

    Move(Cell a, Cell b) : from(a), to(b) {}

    bool operator==(const Move &b) const = default;
};