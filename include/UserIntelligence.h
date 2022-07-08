#pragma once

#include "Intelligence.h"

class UserIntelligence : public Intelligence {
public:
    UserIntelligence(Color color, Cell king_position) : Intelligence(color, king_position) {}

    void make_move(Board &board, History &history, int &last_move_for_50move) override;
};