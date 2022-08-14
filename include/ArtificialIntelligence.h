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
    std::vector<std::vector<Figure>> figures;
    Cell _opponent_king_position;

    struct Features {
        int kings = 0;
        int queens = 0;
        int bishops = 0;
        int knights = 0;
        int rooks = 0;
        int pawns = 0;
        int mobility = 0;
    };

    [[nodiscard]] double evaluate(const Board &board, Move last_move, Color move) const;

    static void undo_move(Board &board, Move last_move, const Figure &old_figure_from, const Figure &old_figure_to);

    std::pair<double, std::pair<Move, Type>> search(Board &board, Move last_move, int depth);

    static Features
    get_features(const std::vector<Figure> &cur_figures, const Board &board, Move last_move, Cell king_position);
};