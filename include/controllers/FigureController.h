#pragma once

#include <vector>
#include "../constants.h"

using Board = std::vector<std::vector<Figure>>;
using History = std::vector<std::pair<std::pair<int, int>, std::pair<int, int> > >;

class FigureController {
public:
    virtual std::vector<std::pair<int, int>> get_moves(std::pair<int, int> coords, const Board &board, History &history,
                                                       const std::pair<int, int> &king_position) = 0;

    virtual void make_move(const std::pair<int, int> &from, const std::pair<int, int> &to, Board &board, History &history,
                           Type promote_to);

    virtual bool is_correct_move(std::pair<std::pair<int, int>, std::pair<int, int>> move_to_check, const Board &board,
                                 History &history, const std::pair<int, int> &king_position) {
        std::vector<std::pair<int, int>> possible_moves = get_moves(move_to_check.first, board, history, king_position);
        for (auto move : possible_moves) {
            if (move == move_to_check.second) return true;
        }
        return false;
    }
};
