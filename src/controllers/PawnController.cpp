#include "../../include/controllers/PawnController.h"
#include "../../include/controllers/KingController.h"

std::vector<Cell> PawnController::get_moves(Cell coords, const Board &board, Move last_move,
                                            const Cell &king_position) {
    int x = coords.x;
    int y = coords.y;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::PAWN)
        return {};

    Color color = board[y][x]._color;
    Color opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    int direction = (color == Color::WHITE) ? 1 : -1;
    int en_passant = (color == Color::WHITE) ? 4 : 3;
    std::vector<Cell> all_possible_moves;
    std::vector<Cell> correct_possible_moves;

    if (board[y + direction][x] == NONE) all_possible_moves.emplace_back(x, y + direction);
    if (!all_possible_moves.empty() && !board[y][x]._is_moved && board[y + 2 * direction][x] == NONE)
        all_possible_moves.emplace_back(x, y + 2 * direction);
    if (x < 7 && board[y + direction][x + 1] != NONE && board[y + direction][x + 1]._color != color)
        all_possible_moves.emplace_back(x + 1, y + direction);
    if (x > 0 && board[y + direction][x - 1] != NONE && board[y + direction][x - 1]._color != color)
        all_possible_moves.emplace_back(x - 1, y + direction);

    if (last_move.from.x != -1) {
        int last_move_x = last_move.to.x;
        int last_move_y = last_move.to.y;
        int delta_last_move_y = abs(last_move.from.y - last_move_y);
        if (y == en_passant && board[last_move_y][last_move_x]._type == Type::PAWN && last_move_x == x + 1 &&
            delta_last_move_y == 2)
            all_possible_moves.emplace_back(x + 1, y + direction);
        if (y == en_passant && board[last_move_y][last_move_x]._type == Type::PAWN && last_move_x == x - 1 &&
            delta_last_move_y == 2)
            all_possible_moves.emplace_back(x - 1, y + direction);
    }

    Board board_copy;
    for (auto to : all_possible_moves) {
        board_copy = board;
        make_move({coords, to}, board_copy, last_move, Type::PAWN);
        if (!KingController::is_attacked(king_position, opposite_color, board_copy))
            correct_possible_moves.emplace_back(to);
    }

    return correct_possible_moves;
}