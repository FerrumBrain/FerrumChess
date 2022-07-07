#pragma once

#include "FigureController.h"

class KingController : public FigureController {
public:
    std::vector<std::pair<int, int>> get_moves(std::pair<int, int> coords, const Board &board, History &history,
                                               const std::pair<int, int> &king_position) override;
    bool is_attacked(std::pair<int, int> coords, Color color, const Board &board);
};
