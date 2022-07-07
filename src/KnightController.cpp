#include "../include/controllers/KnightController.h"
#include "../include/controllers/KingController.h"

std::vector<std::pair<int, int>> KnightController::get_moves(std::pair<int, int> coords, const Board &board, History &history,
                                                             const std::pair<int, int> &king_position) {
    int x = coords.first, y = coords.second;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::KNIGHT)
        return {};

    Color color = board[y][x]._color, opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    std::vector<std::pair<int, int>> all_possible_moves, correct_possible_moves;
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            if (abs(dx) + abs(dy) != 3) continue;
            if (x + dx < 0 || x + dx > 7 || y + dy < 0 || y + dy > 7) continue;

            if (board[y + dy][x + dx]._color != color) all_possible_moves.emplace_back(x + dx, y + dy);
        }
    }

    KingController kingController;
    for (auto to : all_possible_moves) {
        Board board_copy = board;
        make_move(coords, to, board_copy, history, Type::EMPTY);
        if (!kingController.is_attacked(king_position, opposite_color, board_copy))
            correct_possible_moves.emplace_back(to);
        history.pop_back();
    }

    return correct_possible_moves;
}