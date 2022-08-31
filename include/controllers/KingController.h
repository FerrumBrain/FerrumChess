#pragma once

#include "FigureController.h"

class KingController : public FigureController {
public:
    std::vector<Cell> get_moves(Cell coords, Board &board, Move last_move,
                                const Cell &king_position, bool can_take_king) override;
    static bool is_attacked(Cell coords, Color color, const Board &board);
};
