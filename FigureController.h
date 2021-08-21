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
                           Type promote_to) {
        int step = (board[from.first][from.second]._color == Color::WHITE) ? 1 : -1;


        if (board[from.second][from.first]._type == Type::PAWN && to.second == 7) {
            board[from.second][from.first]._type = promote_to;
        }

        if (board[from.second][from.first]._type == Type::PAWN && abs(to.first - from.first) == 1 &&
            board[to.second][to.first] == NONE) {
            board[to.second + step][to.first] = NONE;
        }

        if (board[from.second][from.first]._type == Type::KING && abs(to.first - from.first) == 2) {
            int rook_position = (to.first - from.first < 0) ? 0 : 7, delta = (to.first - from.first < 0) ? 1 : -1;
            make_move({rook_position, from.second}, {to.first + delta, to.second}, board, history, promote_to);
            history.pop_back();
        }

        board[to.second][to.first] = board[from.second][from.first];
        board[from.second][from.first] = NONE;
        history.emplace_back(from, to);
        board[to.second][to.first]._coords = to;
    };
};
