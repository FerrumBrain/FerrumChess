#include "QueenController.h"
#include "KingController.h"

vector<pair<int, int>> QueenController::get_moves(pair<int, int> coords, const Board &board, History &history,
                                                  const pair<int, int> &king_position) {
    int x = coords.first, y = coords.second;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::QUEEN)
        return {};

    Color color = board[y][x]._color, opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    vector<pair<int, int>> all_possible_moves, correct_possible_moves;
    vector<bool> closed = {false, false, false, false, false, false, false, false};
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

        if (x + delta >= 0 && x + delta <= 7 && y + delta >= 0 && y + delta <= 7) {
            if (!closed[4]) {
                if (board[y + delta][x + delta]._color != color) {
                    all_possible_moves.emplace_back(x + delta, y + delta);
                }

                if (board[y + delta][x + delta] != NONE) {
                    closed[4] = true;
                }
            }
        }

        if (x + delta >= 0 && x + delta <= 7 && y - delta >= 0 && y - delta <= 7) {
            if (!closed[5]) {
                if (board[y - delta][x + delta]._color != color) {
                    all_possible_moves.emplace_back(x + delta, y - delta);
                }

                if (board[y - delta][x + delta] != NONE) {
                    closed[5] = true;
                }
            }
        }

        if (x - delta >= 0 && x - delta <= 7 && y + delta >= 0 && y + delta <= 7) {
            if (!closed[6]) {
                if (board[y + delta][x - delta]._color != color) {
                    all_possible_moves.emplace_back(x - delta, y + delta);
                }

                if (board[y + delta][x - delta] != NONE) {
                    closed[6] = true;
                }
            }
        }

        if (x - delta >= 0 && x - delta <= 7 && y - delta >= 0 && y - delta <= 7) {
            if (!closed[7]) {
                if (board[y - delta][x - delta]._color != color) {
                    all_possible_moves.emplace_back(x - delta, y - delta);
                }

                if (board[y - delta][x - delta] != NONE) {
                    closed[7] = true;
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