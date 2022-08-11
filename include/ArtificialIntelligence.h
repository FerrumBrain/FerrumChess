#pragma once

#include "Intelligence.h"
#include <random>
#include <chrono>

class ArtificialIntelligence : public Intelligence {
public:
    // Clang-tidy says that this seed will generate predictable random numbers
    // NOLINTNEXTLINE
    ArtificialIntelligence(Color color, Cell king_position) : Intelligence(color, king_position) {
        gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }
    Move make_move(Board &board, Move last_move) override;

private:
    std::mt19937_64 gen;
    std::vector<Figure> figures;
    [[nodiscard]] double evaluate(const Board &board, Move last_move, Color move) const;
    static void undo_move(Board &board, Move last_move, const Figure &old_figure_from, const Figure &old_figure_to);
    [[nodiscard]] static bool is_better(double eval1, double eval2, Color color) ;
    Move search(Board &board, Move last_move);
};