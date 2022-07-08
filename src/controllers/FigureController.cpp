#include "../../include/controllers/FigureController.h"

void FigureController::make_move(const Move &move, Board &board, History &history,
                       Type promote_to) {
    auto [from, to] = move;

    int step = (board[from.y][from.x]._color == Color::WHITE) ? 1 : -1;


    if (board[from.y][from.x]._type == Type::PAWN && to.y == 7) {
        board[from.y][from.x]._type = promote_to;
    }

    if (board[from.y][from.x]._type == Type::PAWN && abs(to.x - from.x) == 1 &&
    board[to.y][to.x] == NONE) {
        board[to.y + step][to.x] = NONE;
    }

    if (board[from.y][from.x]._type == Type::KING && abs(to.x - from.x) == 2) {
        int rook_position = (to.x - from.x < 0) ? 0 : 7, delta = (to.x - from.x < 0) ? 1 : -1;
        make_move({{rook_position, from.y}, {to.x + delta, to.y}}, board, history, promote_to);
        history.pop_back();
    }

    board[to.y][to.x] = board[from.y][from.x];
    board[from.y][from.x] = NONE;
    history.emplace_back(from, to);
    board[to.y][to.x]._coords = to;
    board[to.y][to.x]._is_moved = true;
}