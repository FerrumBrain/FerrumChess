#include "Type.h"
#include "Color.h"
#include <iostream>
#include "Move.h"

struct Figure {
    Type _type;
    Color _color;
    Cell _coords;
    bool _is_moved;

    Figure(Type type, Color color, int x, int y, bool is_moved = false) : _type(type), _color(color), _coords{x, y},
                                                                          _is_moved(is_moved) {
    }

    bool operator==(const Figure &rhs) const {
        return _type == rhs._type && _color == rhs._color && _coords == rhs._coords;
    }
};
