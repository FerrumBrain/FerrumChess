#pragma once

#include "Intelligence.h"
#include <random>
#include <chrono>

class ArtificialIntelligence : public Intelligence {
public:
    ArtificialIntelligence(Color color, Cell king_position) : Intelligence(color, king_position) {
        gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }
    Move make_move(Board &board, Move last_move, int &last_move_for_50move) override;

private:
    std::mt19937_64 gen;
    std::vector<Cell> figures_coords;
    void undo_move(Board &board, Move last_move, const Figure &old_figure);
    double evaluate(Board &board);
    Move search(Board &board, Move last_move);
};