enum class Color : int {
    EMPTY,
    WHITE,
    BLACK
};

inline ostream &operator<<(ostream &os, const Color &c) {
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
