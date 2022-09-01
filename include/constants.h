#include "Figure.h"

// Big Bad "Static storage duration"...
// NOLINTNEXTLINE
const Figure NONE = Figure(Type::EMPTY, Color::EMPTY, -1, -1, true);
const int INF = 10'000'000;
const int BIG_INFINITY = 1'000'000'000;
const int DEPTH = 2 * 3 + 1;
