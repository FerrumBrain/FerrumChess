#pragma once

#include "Intelligence.h"

class ArtificialIntelligence : public Intelligence {
public:
    ArtificialIntelligence(Color color, pair<int, int> king_position) : Intelligence(color, king_position) {}

    void make_move(Board &board, History &history) override;
};