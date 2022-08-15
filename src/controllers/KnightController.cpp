#include "../../include/controllers/KnightController.h"
#include "../../include/controllers/KingController.h"

std::vector<Cell> KnightController::get_moves(Cell coords, Board &board, Move last_move,
                                              const Cell &king_position) {
    int x = coords.x;
    int y = coords.y;
    if (x < 0 || x > 7 || y < 0 || y > 7 || board[y][x]._type != Type::KNIGHT)
        return {};

    Color color = board[y][x]._color;
    Color opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    std::vector<Cell> all_possible_moves;
    std::vector<Cell> correct_possible_moves;
    for (auto[dx, dy] : std::vector<Cell>{
            {2,  1},
            {2,  -1},
            {-2, 1},
            {-2, -1},
            {1,  2},
            {1,  -2},
            {-1, 2},
            {-1, -2}
    }) {
        if (is_correct_cell({x + dx, y + dy}) && board[y + dy][x + dx]._color != color)
            all_possible_moves.emplace_back(x + dx, y + dy);
    }

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