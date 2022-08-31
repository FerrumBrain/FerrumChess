#include "../../include/controllers/PawnController.h"
#include "../../include/controllers/KingController.h"

std::vector<Cell> PawnController::get_moves(Cell coords, Board &board, Move last_move,
                                            const Cell &king_position, bool can_take_king) {
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
            delta_last_move_y == 2 && board[last_move_y][last_move_x]._color == opposite(color))
            all_possible_moves.emplace_back(x + 1, y + direction);
        if (y == en_passant && board[last_move_y][last_move_x]._type == Type::PAWN && last_move_x == x - 1 &&
            delta_last_move_y == 2 && board[last_move_y][last_move_x]._color == opposite(color))
            all_possible_moves.emplace_back(x - 1, y + direction);
    }

    if (!can_take_king) {
        for (auto to : all_possible_moves) {
            Figure from_figure = board[coords.y][coords.x];
            Figure to_figure = board[to.y][to.x];
            make_move({coords, to}, board, last_move, Type::EMPTY);
            if (!KingController::is_attacked(king_position, opposite_color, board))
                correct_possible_moves.emplace_back(to);
            board[coords.y][coords.x] = from_figure;
            board[to.y][to.x] = to_figure;
            if (abs(to.x - coords.x) == 1 && to_figure._type == Type::EMPTY) {
                board[to.y - direction][to.x] = Figure(Type::PAWN, opposite_color, to.x, to.y - direction, true);
            }
        }

        return correct_possible_moves;
    }
    return all_possible_moves;
}