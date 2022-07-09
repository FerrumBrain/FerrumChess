#include "../include/ArtificialIntelligence.h"

Move ArtificialIntelligence::make_move(Board &board, Move last_move, int &last_move_for_50move) {
    std::vector<Move> possible_moves;
    std::vector<Cell> current_moves;
    figures_coords.clear();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]._color == _color) {
                figures_coords.emplace_back(j, i);
            }
        }
    }

    for (auto cell: figures_coords) {
        current_moves = controller_container[board[cell.y][cell.x]._type]->get_moves(cell, board, last_move,
                                                                                     _king_position);
        for (auto to : current_moves) {
            possible_moves.emplace_back(cell, to);
        }
    }

    unsigned long long index = gen() % possible_moves.size();
    auto[from, to] = possible_moves[index];
    Type type = board[from.y][from.x]._type;
    Type promote_to = Type::EMPTY;
    if (type == Type::PAWN && to.y == 7) {
        promote_to = Type(gen() % 6 + 1);
    }

    controller_container[type]->make_move({from, to}, board, last_move, promote_to);
    if (board[to.y][to.x]._type == Type::KING)
        _king_position = to;

    return {from, to};
}