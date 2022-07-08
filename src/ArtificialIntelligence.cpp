#include "../include/ArtificialIntelligence.h"

void ArtificialIntelligence::make_move(Board &board, History &history, int &last_move_for_50move) {
    std::vector<Move> possible_moves;
    std::vector<Cell> current_moves;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j]._color == _color){
                current_moves = controller_container[board[i][j]._type]->get_moves({j, i}, board, history, _king_position);
                for(auto to : current_moves){
                    possible_moves.push_back({{j, i}, to});
                }
            }
        }
    }

    int index = gen() % possible_moves.size();
    auto [from, to] = possible_moves[index];
    Type type = board[from.y][from.x]._type, promote_to = Type::EMPTY;
    if(type == Type::PAWN && to.y == 7) {
        promote_to = Type(gen() % 6 + 1);
    }

    if(board[from.y][from.x]._type == Type::PAWN || board[to.y][to.x]._type != Type::EMPTY) {
        last_move_for_50move = static_cast<int>(history.size()) + 1;
    }

    controller_container[type]->make_move({from, to}, board, history, promote_to);
    if(board[to.y][to.x]._type == Type::KING)
        _king_position = to;
}

void ArtificialIntelligence::undo_move(Board &board, History &history, const Figure &old_figure) {

}

double ArtificialIntelligence::evaluate(Board &board) {
    return 0;
}

Move ArtificialIntelligence::search(Board &board, History &history) {
    return Move();
}

