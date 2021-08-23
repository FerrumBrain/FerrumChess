#pragma once

#include "Intelligence.h"
#include <random>
#include <ctime>

class ArtificialIntelligence : public Intelligence {
public:
    mt19937 gen;
    ArtificialIntelligence(Color color, pair<int, int> king_position) : Intelligence(color, king_position) {
        long long seed = time(nullptr);
        gen.seed(seed);
    }

    void make_move(Board &board, History &history) override;
};