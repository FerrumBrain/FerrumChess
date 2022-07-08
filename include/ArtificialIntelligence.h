#pragma once

#include "Intelligence.h"
#include <random>
#include <ctime>

class ArtificialIntelligence : public Intelligence {
public:
    std::mt19937 gen;
    ArtificialIntelligence(Color color, Cell king_position) : Intelligence(color, king_position) {
        long long seed = time(nullptr);
        gen.seed(seed);
    }

    void make_move(Board &board, History &history, int &last_move_for_50move) override;

private:
    void undo_move(Board &board, History &history, const Figure &old_figure);
    double evaluate(Board &board);
    std::vector<Figure> figures;
    Move search(Board &board, History &history);
};