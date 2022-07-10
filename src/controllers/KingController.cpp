#include "../../include/controllers/KingController.h"

bool is_attacked_by_knight(int x, int y, Color color, const Board &board) {
    std::vector<Cell> deltas = {
            {2,  1},
            {2,  -1},
            {-2, 1},
            {-2, -1},
            {1,  2},
            {1,  -2},
            {-1, 2},
            {-1, -2}
    };
    return std::ranges::any_of(deltas.begin(), deltas.end(), [&](Cell a) {
        auto[dx, dy] = a;
        return is_correct_cell({x + dx, y + dy}) &&
               board[y + dy][x + dx]._type == Type::KNIGHT && board[y + dy][x + dx]._color == color;
    });
}

bool KingController::is_attacked(Cell coords, Color color, const Board &board) {
    int x = coords.x;
    int y = coords.y;
    if (!is_correct_cell({x, y})) return false;

    if ((color == Color::BLACK &&
         (y != 7 && (x >= 1 && board[y + 1][x - 1]._type == Type::PAWN && board[y + 1][x - 1]._color == color ||
                     x <= 6 && board[y + 1][x + 1]._type == Type::PAWN && board[y + 1][x + 1]._color == color)))
        || (color == Color::WHITE &&
            (y != 0 && (x >= 1 && board[y - 1][x - 1]._type == Type::PAWN && board[y - 1][x - 1]._color == color ||
                        x <= 6 && board[y - 1][x + 1]._type == Type::PAWN && board[y - 1][x + 1]._color == color))))
        return true;

    if (is_attacked_by_knight(x, y, color, board)) {
        return true;
    }

    std::vector<bool> closed(8, false);
    Type current_type;
    for (int delta = 1; delta <= 7; delta++) {
        int i = 0;
        for (auto[dx, dy] : std::vector<Cell>{
                {-1, -1},
                {-1, 0},
                {-1, 1},
                {0,  -1},
                {0,  1},
                {1,  -1},
                {1,  0},
                {1,  1}
        }) {
            current_type = (dx * dx + dy * dy == 1) ? Type::ROOK : Type::BISHOP;
            if (is_correct_cell({x + dx * delta, y + dy * delta}) && !closed[i] &&
                board[y + dy * delta][x + dx * delta]._color == color &&
                (board[y + dy * delta][x + dx * delta]._type == Type::QUEEN ||
                 board[y + dy * delta][x + dx * delta]._type == current_type ||
                 (board[y + dy * delta][x + dx * delta]._type == Type::KING && delta == 1))) {
                return true;
            }

            closed[i] = is_correct_cell({x + dx * delta, y + dy * delta}) && !closed[i] && board[y][x + delta] != NONE;
            i++;
        }
    }
    return false;
}

std::vector<Cell> KingController::get_moves(Cell coords, Board board, Move last_move,
                                            const Cell &king_position) {
    int x = coords.x;
    int y = coords.y;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::KING)
        return {};

    Color color = board[y][x]._color;
    Color opponent_color = (color == Color::BLACK) ? Color::WHITE : Color::BLACK;
    std::vector<Cell> all_possible_moves;
    std::vector<Cell> correct_possible_moves;
    for (auto[dx, dy] : std::vector<Cell>{
            {-1, -1},
            {-1, 0},
            {-1, 1},
            {0,  -1},
            {0,  1},
            {1,  -1},
            {1,  0},
            {1,  1}
    }) {
        if (!is_correct_cell({x + dx, y + dy})) continue;

        if (board[y + dy][x + dx]._color != color) all_possible_moves.emplace_back(x + dx, y + dy);
    }

    if (!board[y][x]._is_moved && !is_attacked(king_position, opponent_color, board)) {
        if (!board[y][7]._is_moved && board[y][5] == NONE && board[y][6] == NONE &&
            !is_attacked({5, y}, opponent_color, board) && !is_attacked({6, y}, opponent_color, board))
            all_possible_moves.emplace_back(6, y);
        if (!board[y][0]._is_moved && board[y][1] == NONE && board[y][2] == NONE &&
            board[y][3] == NONE && !is_attacked({2, y}, opponent_color, board) &&
            !is_attacked({3, y}, opponent_color, board))
            all_possible_moves.emplace_back(2, y);
    }

    for (auto to : all_possible_moves) {
        if (abs(to.x - coords.x) == 2) continue;

        Figure from_figure = board[coords.y][coords.x];
        Figure to_figure = board[to.y][to.x];
        make_move({coords, to}, board, last_move, Type::EMPTY);
        if (!is_attacked(to, opponent_color, board)) {
            correct_possible_moves.emplace_back(to);
        }
        board[coords.y][coords.x] = from_figure;
        board[to.y][to.x] = to_figure;
    }

    return correct_possible_moves;
}