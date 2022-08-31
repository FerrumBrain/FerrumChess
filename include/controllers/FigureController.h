#pragma once

#include <vector>
#include <algorithm>
#include "../constants.h"

using Board = std::vector<std::vector<Figure>>;
using History = std::vector<Move>;

bool is_correct_cell(Cell cell);

class FigureController {
public:
    virtual std::vector<Cell> get_moves(Cell coords, Board &board, Move last_move,
                                        const Cell &king_position, bool can_take_king) = 0;

    void make_move(Move move, Board &board, Move last_move,
                   Type promote_to);

    bool is_correct_move(Move move_to_check, Board &board,
                         Move last_move, const Cell &king_position) {
        std::vector<Cell> possible_moves = get_moves(move_to_check.from, board, last_move, king_position, false);
        return std::ranges::any_of(possible_moves.begin(), possible_moves.end(), [&](Cell a) {
            return (a == move_to_check.to);
        });
    }

    virtual ~FigureController() = default;
};
