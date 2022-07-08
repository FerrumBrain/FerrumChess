#pragma once

#include "FigureController.h"

class KingController : public FigureController {
public:
    std::vector<Cell> get_moves(Cell coords, const Board &board, History &history,
                                const Cell &king_position) override;
    bool is_attacked(Cell coords, Color color, const Board &board);
};
