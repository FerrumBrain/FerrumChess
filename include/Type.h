#include <iostream>
#include <cassert>

enum class Type : int {
    EMPTY = 0,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6
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
