#include "Type.h"
#include "Color.h"
#include <iostream>

struct Figure {
    Type _type;
    Color _color;
    std::pair<int, int> _coords;
    bool _is_moved;

    Figure(Type type, Color color, int x, int y, bool is_moved = false) {
        _type = type;
        _color = color;
        _coords = {x, y};
        _is_moved = is_moved;
    }
};

inline bool operator==(const Figure& lhs, const Figure &rhs) {
    return lhs._type == rhs._type && lhs._color == rhs._color && lhs._coords == rhs._coords;
}

inline bool operator!=(const Figure& lhs, const Figure &rhs) {
    return lhs._type != rhs._type && lhs._color != rhs._color &&  lhs._coords != rhs._coords;
}