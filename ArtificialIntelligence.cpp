#include "ArtificialIntelligence.h"

void ArtificialIntelligence::make_move(Board &board, History &history) {
    vector<pair<pair<int, int>, pair<int, int>>> possible_moves;
    vector<pair<int, int>> current_moves;
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
    Type type = board[possible_moves[index].first.second][possible_moves[index].first.first]._type, promote_to = Type::EMPTY;
    if(type == Type::PAWN && possible_moves[index].second.second == 7) {
        promote_to = Type(gen() % 6 + 1);
    }
    controller_container[type]->make_move(possible_moves[index].first, possible_moves[index].second, board, history, promote_to);
    if(board[possible_moves[index].second.second][possible_moves[index].second.first]._type == Type::KING)
        _king_position = possible_moves[index].second;
}