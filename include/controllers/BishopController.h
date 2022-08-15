#pragma once

#include "FigureController.h"

class BishopController : public FigureController {
public:
    std::vector<Cell> get_moves(Cell coords, Board &board, Move last_move,
                                const Cell &king_position) override;
};