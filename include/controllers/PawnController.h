#pragma once

#include "FigureController.h"

class PawnController : public FigureController {
public:
    std::vector<Cell> get_moves(Cell coords, const Board &board, Move last_move,
                                const Cell &king_position) override;
};
