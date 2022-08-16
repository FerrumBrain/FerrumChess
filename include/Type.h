#include <iostream>
#include <cassert>

enum class Type : int {
    EMPTY = -1,
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
};

inline std::ostream &operator<<(std::ostream &os, const Type &t) {
    switch(t){
        case Type::EMPTY:
            os << "-";
            break;
        case Type::PAWN:
            os << "P";
            break;
        case Type::KNIGHT:
            os << "N";
            break;
        case Type::BISHOP:
            os << "B";
            break;
        case Type::ROOK:
            os << "R";
            break;
        case Type::QUEEN:
            os << "Q";
            break;
        case Type::KING:
            os << "K";
            break;
        default:
            assert("Something went wrong");
    }
    return os;
}
