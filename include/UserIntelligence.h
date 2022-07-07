#pragma once

#include "Intelligence.h"

class UserIntelligence : public Intelligence {
public:
    UserIntelligence(Color color, std::pair<int, int> king_position) : Intelligence(color, king_position) {}

    void make_move(Board &board, History &history) override;
};