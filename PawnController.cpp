#include "PawnController.h"
#include "KingController.h"

vector<pair<int, int>> PawnController::get_moves(pair<int, int> coords, const Board &board, History &history,
                                                 const pair<int, int> &king_position) {
    int x = coords.first, y = coords.second;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::PAWN)
        return {};

    Color color = board[y][x]._color, opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    int direction = (color == Color::WHITE) ? 1 : -1, en_passant = (color == Color::WHITE) ? 4 : 3;
    vector<pair<int, int>> all_possible_moves, correct_possible_moves;

    if (board[y + direction][x] == NONE) all_possible_moves.emplace_back(x, y + direction);
    if (!all_possible_moves.empty() && !board[y][x]._is_moved && board[y + 2 * direction][x] == NONE)
        all_possible_moves.emplace_back(x, y + 2 * direction);
    if (x < 7 && board[y + direction][x + 1] != NONE && board[y + direction][x + 1]._color != color)
        all_possible_moves.emplace_back(x + 1, y + direction);
    if (x > 0 && board[y + direction][x - 1] != NONE && board[y + direction][x - 1]._color != color)
        all_possible_moves.emplace_back(x - 1, y + direction);

    if (!history.empty()) {
        int last_move_x = history[history.size() - 1].second.first,
                last_move_y = history[history.size() - 1].second.second,
                delta_last_move_y = abs(history[history.size() - 1].first.second - last_move_y);
        if (y == en_passant && board[last_move_y][last_move_x]._type == Type::PAWN && last_move_x == x + 1 &&
            delta_last_move_y == 2)
            all_possible_moves.emplace_back(x + 1, y + direction);
        if (y == en_passant && board[last_move_y][last_move_x]._type == Type::PAWN && last_move_x == x - 1 &&
            delta_last_move_y == 2)
            all_possible_moves.emplace_back(x - 1, y + direction);
    }

    KingController kingController;
    Board board_copy;
    for (auto to : all_possible_moves) {
        board_copy = board;
        make_move(coords, to, board_copy, history, Type::PAWN);
        if (!kingController.is_attacked(king_position, opposite_color, board_copy))
            correct_possible_moves.emplace_back(to);
        history.pop_back();
    }

    return correct_possible_moves;
}