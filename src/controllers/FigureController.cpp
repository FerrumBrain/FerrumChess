#include "../../include/controllers/FigureController.h"

bool is_correct_cell(Cell cell) {
    return 0 <= cell.x && cell.x <= 7 && 0 <= cell.y && cell.y <= 7;
}

void FigureController::make_move(Move move, Board &board, Move last_move,
                       Type promote_to) {
    auto [from, to] = move;

    int step = (board[from.y][from.x]._color == Color::WHITE) ? -1 : 1;


    if (board[from.y][from.x]._type == Type::PAWN && to.y % 7 == 0) {
        board[from.y][from.x]._type = promote_to;
    }

    if (board[from.y][from.x]._type == Type::PAWN && abs(to.x - from.x) == 1 &&
    board[to.y][to.x] == NONE) {
        board[to.y + step][to.x] = NONE;
    }

    if (board[from.y][from.x]._type == Type::KING && abs(to.x - from.x) == 2) {
        int rook_position = (to.x - from.x < 0) ? 0 : 7;
        int delta = (to.x - from.x < 0) ? 1 : -1;
        make_move({{rook_position, from.y}, {to.x + delta, to.y}}, board, last_move, promote_to);
    }

    board[to.y][to.x] = board[from.y][from.x];
    board[from.y][from.x] = NONE;
    board[to.y][to.x]._coords = to;
    board[to.y][to.x]._is_moved = true;
}