enum class Color : int {
    EMPTY,
    WHITE,
    BLACK
};

inline std::ostream &operator<<(std::ostream &os, const Color &c) {
    switch (c) {
        case (Color::EMPTY):
            os << "-";
            break;
        case (Color::WHITE):
            os << "W";
            break;
        case (Color::BLACK):
            os << "B";
            break;
    }
    return os;
}

inline std::string to_string(Color c) {
    switch (c) {
        case (Color::EMPTY):
            return "None";
        case (Color::WHITE):
            return "White";
        case (Color::BLACK):
            return "Black";
    }
}
