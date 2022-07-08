#include "../include/UserIntelligence.h"
#include "../include/UIController.h"

void UserIntelligence::make_move(Board &board, History &history, int &last_move_for_50move) {
    while (true) {
        auto [from, to] = UIController::get_users_move();
        if (board[from.y][from.x]._type != Type::EMPTY) {
            if (controller_container[board[from.y][from.x]._type]->is_correct_move({from, to}, board, history,
                                                                                           _king_position)) {
                Type promote_to = Type::EMPTY;
                if (to.y == 7 && board[from.y][from.x]._type == Type::PAWN) {
                    promote_to = UIController::promote_to();
                }

                if(board[from.y][from.x]._type == Type::PAWN || board[to.y][to.x]._type != Type::EMPTY) {
                    last_move_for_50move = static_cast<int>(history.size()) + 1;
                }

                controller_container[board[from.y][from.x]._type]->make_move({from, to}, board, history,
                                                                                      promote_to);
                if(board[to.y][to.x]._type == Type::KING)
                    _king_position = to;
                return;
            }
        }
        UIController::impossible_move();
    }
}