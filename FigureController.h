#pragma once

#include <vector>
#include "constants.h"

using namespace std;

using Board = vector<vector<Figure>>;
using History = vector<pair<pair<int, int>, pair<int, int> > >;

class FigureController {
public:
    virtual vector<pair<int, int>> get_moves(pair<int, int> coords, const Board &board, History &history,
                                             const pair<int, int> &king_position) = 0;

    virtual void make_move(const pair<int, int> &from, const pair<int, int> &to, Board &board, History &history,
                           Type promote_to);

    virtual bool is_correct_move(pair<pair<int, int>, pair<int, int>> move_to_check, const Board &board,
                                 History &history, const pair<int, int> &king_position) {
        vector<pair<int, int>> possible_moves = get_moves(move_to_check.first, board, history, king_position);
        for (auto move : possible_moves) {
            if (move == move_to_check.second) return true;
        }
        return false;
    }
};
