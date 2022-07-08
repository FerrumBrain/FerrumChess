#pragma once

#include <vector>
#include "../constants.h"

using Board = std::vector<std::vector<Figure>>;
using History = std::vector<Move>;

class FigureController {
public:
    virtual std::vector<Cell> get_moves(Cell coords, const Board &board, History &history,
                                        const Cell &king_position) = 0;

    virtual void make_move(const Move &move, Board &board, History &history,
                           Type promote_to);

    virtual bool is_correct_move(Move move_to_check, const Board &board,
                                 History &history, const Cell &king_position) {
        std::vector<Cell> possible_moves = get_moves(move_to_check.from, board, history, king_position);
        for (auto move : possible_moves) {
            if (move == move_to_check.to) return true;
        }
        return false;
    }
};
