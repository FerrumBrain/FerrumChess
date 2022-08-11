enum class Color : int {
    EMPTY,
    WHITE,
    BLACK
};

inline Color opposite(Color color) {
    return color == Color::WHITE ? Color::BLACK : Color::WHITE;
}

inline std::ostream &operator<<(std::ostream &os, const Color &c) {
    switch (c) {
        case Color::EMPTY:
            os << "-";
            break;
        case Color::WHITE:
            os << "W";
            break;
        case Color::BLACK:
            os << "B";
            break;
        default:
            assert("Something went wrong");
    }
    return os;
}

inline std::string to_string(Color c) {
    switch (c) {
        case Color::EMPTY:
            return "None";
        case Color::WHITE:
            return "White";
        case Color::BLACK:
            return "Black";
        default:
            assert("Something went wrong");
    }
}
