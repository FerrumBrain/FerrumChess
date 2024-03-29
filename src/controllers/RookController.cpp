#include "../../include/controllers/RookController.h"
#include "../../include/controllers/KingController.h"

std::vector<Cell> RookController::get_moves(Cell coords, Board &board, Move last_move,
                                            const Cell &king_position, bool can_take_king) {
    int x = coords.x;
    int y = coords.y;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::ROOK)
        return {};

    Color color = board[y][x]._color;
    Color opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    std::vector<Cell> all_possible_moves;
    std::vector<Cell> correct_possible_moves;
    std::vector<bool> closed(4, false);
    for (int delta = 1; delta <= 7; delta++) {
        int i = 0;
        for (auto [dx, dy] : std::vector<Cell> {
            {-1, 0},
            {0, -1},
            {0, 1},
            {1, 0}
        }) {
            if (is_correct_cell({x + dx * delta, y + dy * delta}) && !closed[i] &&
            board[y + dy * delta][x + dx * delta]._color != color) {
                all_possible_moves.emplace_back(x + dx * delta, y + dy * delta);
            }

            closed[i] = !is_correct_cell({x + dx * delta, y + dy * delta}) || closed[i] || board[y + delta * dy][x + delta * dx] != NONE;
            i++;
        }
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
        }

        return correct_possible_moves;
    }
    return all_possible_moves;
}