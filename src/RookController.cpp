#include "../include/controllers/RookController.h"
#include "../include/controllers/KingController.h"

std::vector<std::pair<int, int>> RookController::get_moves(std::pair<int, int> coords, const Board &board, History &history,
                                                           const std::pair<int, int> &king_position) {
    int x = coords.first, y = coords.second;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::ROOK)
        return {};

    Color color = board[y][x]._color, opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    std::vector<std::pair<int, int>> all_possible_moves, correct_possible_moves;
    std::vector<bool> closed = {false, false, false, false};
    for (int delta = 1; delta <= 7; delta++) {
        if (x + delta >= 0 && x + delta <= 7) {
            if (!closed[0]) {
                if (board[y][x + delta]._color != color) {
                    all_possible_moves.emplace_back(x + delta, y);
                }

                if (board[y][x + delta] != NONE) {
                    closed[0] = true;
                }
            }
        }

        if (y - delta >= 0 && y - delta <= 7) {
            if (!closed[1]) {
                if (board[y - delta][x]._color != color) {
                    all_possible_moves.emplace_back(x, y - delta);
                }

                if (board[y - delta][x] != NONE) {
                    closed[1] = true;
                }
            }
        }

        if (y + delta >= 0 && y + delta <= 7) {
            if (!closed[2]) {
                if (board[y + delta][x]._color != color) {
                    all_possible_moves.emplace_back(x, y + delta);
                }

                if (board[y + delta][x] != NONE) {
                    closed[2] = true;
                }
            }
        }

        if (x - delta >= 0 && x - delta <= 7) {
            if (!closed[3]) {
                if (board[y][x - delta]._color != color) {
                    all_possible_moves.emplace_back(x - delta, y);
                }

                if (board[y][x - delta] != NONE) {
                    closed[3] = true;
                }
            }
        }
    }

    KingController kingController;
    Board board_copy;
    for (auto to : all_possible_moves) {
        board_copy = board;
        make_move(coords, to, board_copy, history, Type::EMPTY);
        if (!kingController.is_attacked(king_position, opposite_color, board_copy))
            correct_possible_moves.emplace_back(to);
        history.pop_back();
    }

    return correct_possible_moves;
}