#include "../../include/controllers/KingController.h"

bool KingController::is_attacked(Cell coords, Color color, const Board &board) {
    int x = coords.x, y = coords.y;
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if (color == Color::BLACK) {
        if (y != 7) {
            if (x >= 1)
                if (board[y + 1][x - 1]._type == Type::PAWN && board[y + 1][x - 1]._color == color)
                    return true;

            if (x <= 6)
                if (board[y + 1][x + 1]._type == Type::PAWN && board[y + 1][x + 1]._color == color)
                    return true;
        }
    } else {
        if (y != 7) {
            if (x >= 1)
                if (board[y - 1][x - 1]._type == Type::PAWN && board[y - 1][x - 1]._color == color)
                    return true;

            if (x <= 6)
                if (board[y - 1][x + 1]._type == Type::PAWN && board[y - 1][x + 1]._color == color)
                    return true;
        }
    }


    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            if (x + dx < 0 || x + dx > 7 || y + dy < 0 || y + dy > 7) continue;
            if (board[y + dy][x + dx]._type == Type::KING && board[y + dy][x + dx]._color == color) return true;
        }
    }


    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            if (abs(dx) + abs(dy) != 3) continue;
            if (x + dx < 0 || x + dx > 7 || y + dy < 0 || y + dy > 7) continue;
            if (board[y + dy][x + dx]._type == Type::KNIGHT && board[y + dy][x + dx]._color == color) return true;
        }
    }

    std::vector<bool> closed = {false, false, false, false, false, false, false, false};
    for (int delta = 1; delta <= 7; delta++) {
        if (x + delta >= 0 && x + delta <= 7) {
            if (!closed[0]) {
                if (board[y][x + delta]._color == color &&
                    (board[y][x + delta]._type == Type::QUEEN || board[y][x + delta]._type == Type::ROOK)) {
                    return true;
                }

                if (board[y][x + delta] != NONE) {
                    closed[0] = true;
                }
            }
        }

        if (y - delta >= 0 && y - delta <= 7) {
            if (!closed[1]) {
                if (board[y - delta][x]._color == color &&
                    (board[y - delta][x]._type == Type::QUEEN || board[y - delta][x]._type == Type::ROOK)) {
                    return true;
                }

                if (board[y - delta][x] != NONE) {
                    closed[1] = true;
                }
            }
        }

        if (y + delta >= 0 && y + delta <= 7) {
            if (!closed[2]) {
                if (board[y + delta][x]._color == color &&
                    (board[y + delta][x]._type == Type::QUEEN || board[y + delta][x]._type == Type::ROOK)) {
                    return true;
                }

                if (board[y + delta][x] != NONE) {
                    closed[2] = true;
                }
            }
        }

        if (x - delta >= 0 && x - delta <= 7) {
            if (!closed[3]) {
                if (board[y][x - delta]._color == color &&
                    (board[y][x - delta]._type == Type::QUEEN || board[y][x - delta]._type == Type::ROOK)) {
                    return true;
                }

                if (board[y][x - delta] != NONE) {
                    closed[3] = true;
                }
            }
        }

        if (x + delta >= 0 && x + delta <= 7 && y + delta >= 0 && y + delta <= 7) {
            if (!closed[4]) {
                if (board[y + delta][x + delta]._color == color && (board[y + delta][x + delta]._type == Type::QUEEN ||
                                                                    board[y + delta][x + delta]._type ==
                                                                    Type::BISHOP)) {
                    return true;
                }

                if (board[y + delta][x + delta] != NONE) {
                    closed[4] = true;
                }
            }
        }

        if (x + delta >= 0 && x + delta <= 7 && y - delta >= 0 && y - delta <= 7) {
            if (!closed[5]) {
                if (board[y - delta][x + delta]._color == color && (board[y - delta][x + delta]._type == Type::QUEEN ||
                                                                    board[y - delta][x + delta]._type ==
                                                                    Type::BISHOP)) {
                    return true;
                }

                if (board[y - delta][x + delta] != NONE) {
                    closed[5] = true;
                }
            }
        }

        if (x - delta >= 0 && x - delta <= 7 && y + delta >= 0 && y + delta <= 7) {
            if (!closed[6]) {
                if (board[y + delta][x - delta]._color == color && (board[y + delta][x - delta]._type == Type::QUEEN ||
                                                                    board[y + delta][x - delta]._type ==
                                                                    Type::BISHOP)) {
                    return true;
                }

                if (board[y + delta][x - delta] != NONE) {
                    closed[6] = true;
                }
            }
        }

        if (x - delta >= 0 && x - delta <= 7 && y - delta >= 0 && y - delta <= 7) {
            if (!closed[7]) {
                if (board[y - delta][x - delta]._color == color && (board[y - delta][x - delta]._type == Type::QUEEN ||
                                                                    board[y - delta][x - delta]._type ==
                                                                    Type::BISHOP)) {
                    return true;
                }

                if (board[y - delta][x - delta] != NONE) {
                    closed[7] = true;
                }
            }
        }
    }
    return false;
}

std::vector<Cell> KingController::get_moves(Cell coords, const Board &board, History &history,
                                            const Cell &king_position) {
    int x = coords.x, y = coords.y;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::KING)
        return {};

    Color color = board[y][x]._color, opponent_color = (color == Color::BLACK) ? Color::WHITE : Color::BLACK;
    std::vector<Cell> all_possible_moves, correct_possible_moves;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            if (x + dx < 0 || x + dx > 7 || y + dy < 0 || y + dy > 7) continue;

            if (board[y + dy][x + dx]._color != color) all_possible_moves.emplace_back(x + dx, y + dy);
        }
    }

    if (!board[y][x]._is_moved && !is_attacked(king_position, opponent_color, board)) {
        if (!board[y][7]._is_moved && board[y][5] == NONE && board[y][6] == NONE) {
            if (!is_attacked({5, y}, opponent_color, board) && !is_attacked({6, y}, opponent_color, board))
                all_possible_moves.emplace_back(6, y);
        }
        if (!board[y][0]._is_moved) {
            if (!board[y][0]._is_moved && board[y][1] == NONE && board[y][2] == NONE && board[y][3] == NONE) {
                if (!is_attacked({2, y}, opponent_color, board) && !is_attacked({3, y}, opponent_color, board))
                    all_possible_moves.emplace_back(2, y);
            }
        }
    }

    for (auto to : all_possible_moves) {
        Board board_copy = board;
        make_move({coords, to}, board_copy, history, Type::EMPTY);
        if (!is_attacked(to, opponent_color, board_copy))
            correct_possible_moves.emplace_back(to);
        history.pop_back();
    }

    return correct_possible_moves;
}