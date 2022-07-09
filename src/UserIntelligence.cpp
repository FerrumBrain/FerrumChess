#include "../include/UserIntelligence.h"
#include "../include/UIController.h"

Move UserIntelligence::make_move(Board &board, Move last_move, int &last_move_for_50move) {
    while (true) {
        auto[from, to] = UIController::get_users_move();
        if (board[from.y][from.x]._type != Type::EMPTY &&
            controller_container[board[from.y][from.x]._type]->is_correct_move({from, to}, board, last_move,
                                                                               _king_position)) {
            Type type_to_promote = Type::EMPTY;
            if (to.y == 7 && board[from.y][from.x]._type == Type::PAWN) {
                type_to_promote = UIController::promote_to();
            }

            controller_container[board[from.y][from.x]._type]->make_move({from, to}, board, last_move,
                                                                         type_to_promote);
            if (board[to.y][to.x]._type == Type::KING)
                _king_position = to;
            return {from, to};
        }
        UIController::impossible_move();
    }
}