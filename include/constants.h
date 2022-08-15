#include "Figure.h"

// Big Bad "Static storage duration"...
// NOLINTNEXTLINE
const Figure NONE = Figure(Type::EMPTY, Color::EMPTY, -1, -1, true);
const double INF = 1'000'000;
const double BIG_INFINITY = 1'000'000'000;
const int DEPTH = 5;
