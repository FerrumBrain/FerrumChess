#pragma once

#include "FigureController.h"

class BishopController : public FigureController {
public:
    std::vector<Cell> get_moves(Cell coords, const Board &board, History &history,
                                const Cell &king_position) override;
};