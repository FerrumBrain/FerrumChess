#pragma once

#include "FigureController.h"

class KingController : public FigureController {
public:
    vector<pair<int, int>> get_moves(pair<int, int> coords, const Board &board, History &history,
                                     const pair<int, int> &king_position) override;
    bool is_attacked(pair<int, int> coords, Color color, const Board &board);
};
