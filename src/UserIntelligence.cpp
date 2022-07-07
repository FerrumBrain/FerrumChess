#include "../include/UserIntelligence.h"

void UserIntelligence::make_move(Board &board, History &history, int &last_move_for_50move) {
    std::pair<int, int> from, to;
    bool flag;
    std::string str, str_raw;
    int i;
    while (true) {
        i = 0, flag = false;
        std::cout << "From-square:" << std::endl;
        while (!flag) {
            str = "";
            if (i != 0) std::cout << "Wrong format, try again" << std::endl;
            i++;
            getline(std::cin, str_raw);
            for (char j : str_raw) {
                if (j != ' ') str += j;
            }
            if (str.size() != 2) continue;
            if (str[0] >= 'a' && str[0] <= 'h')
                if (str[1] >= '1' && str[1] <= '8')
                    flag = true;
        }
        std::cout << std::endl;
        from = {(int) (str[0] - 'a'), (int) (str[1] - '1')};
        flag = false, i = 0;
        std::cout << "To-square:" << std::endl;
        while (!flag) {
            str = "";
            if (i != 0) std::cout << "Wrong format, try again" << std::endl;
            i++;
            getline(std::cin, str_raw);
            for (char j : str_raw) {
                if (j != ' ') str += j;
            }
            if (str.size() != 2) continue;
            if (str[0] >= 'a' && str[0] <= 'h')
                if (str[1] >= '1' && str[1] <= '8')
                    flag = true;
        }
        std::cout << std::endl;
        to = {(int) (str[0] - 'a'), (int) (str[1] - '1')};
        if (board[from.second][from.first]._type != Type::EMPTY) {
            if (controller_container[board[from.second][from.first]._type]->is_correct_move({from, to}, board, history,
                                                                                           _king_position)) {
                if (to.second == 7 && board[from.second][from.first]._type == Type::PAWN) {
                    std::cout << "Promote to:" << std::endl;
                    flag = false, i = 0;
                    while (!flag) {
                        str = "";
                        if (i != 0) std::cout << "Wrong format, try again" << std::endl;
                        i++;
                        getline(std::cin, str_raw);
                        for (char j : str_raw) {
                            if (j != ' ') str += j;
                        }
                        if (str.size() != 1) continue;
                        flag = str[0] == 'N' || str[0] == 'B' || str[0] == 'R' || str[0] == 'Q' ||
                               str[0] == 'n' || str[0] == 'b' || str[0] == 'r' || str[0] == 'q';
                    }
                    std::cout << std::endl;
                }
                Type promote_to = Type::EMPTY;
                if (str == "N" || str == "n") promote_to = Type::KNIGHT;
                if (str == "B" || str == "b") promote_to = Type::BISHOP;
                if (str == "R" || str == "r") promote_to = Type::ROOK;
                if (str == "Q" || str == "q") promote_to = Type::QUEEN;

                if(board[from.second][from.first]._type == Type::PAWN || board[to.second][to.first]._type != Type::EMPTY) {
                    last_move_for_50move = static_cast<int>(history.size()) + 1;
                }

                controller_container[board[from.second][from.first]._type]->make_move(from, to, board, history,
                                                                                      promote_to);
                if(board[to.second][to.first]._type == Type::KING)
                    _king_position = to;
                return;
            }
        }
        std::cout << "Impossible move, try again" << std::endl;
    }
}