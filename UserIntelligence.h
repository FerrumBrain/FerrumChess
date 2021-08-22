#pragma once

#include "Intelligence.h"

class UserIntelligence : public Intelligence {
public:
    explicit UserIntelligence(Color color, pair<int, int> king_position) : Intelligence(color, king_position) {}

    void make_move(Board &board, History &history) override;
};