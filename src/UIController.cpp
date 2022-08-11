#include "../include/UIController.h"

Mode UIController::start_game() {
    std::cout
            << R"(If you want to play new game, type "N". If you want to play from your own position, type "P")"
            << std::endl;
    std::string input;
    std::string raw_input;
    while (true) {
        input = "";
        getline(std::cin, raw_input);
        for (char c: raw_input) {
            if (isspace(c) != 0) continue;
            input += c;
        }
        if (input == "N") {
            return Mode::NEW_GAME;
        }
        if (input == "P") {
            return Mode::FEN_GAME;
        }
    }
}

Color UIController::choose_color() {
    std::cout << R"(If you want to play with white pieces, type "W". Else type "B")" << std::endl;
    std::string input;
    std::string raw_input;
    while (true) {
        input = "";
        getline(std::cin, raw_input);
        for (char c: raw_input) {
            if (c == ' ') continue;
            input += c;
        }
        if (input == "W") {
            return Color::WHITE;
        }
        if (input == "B") {
            return Color::BLACK;
        }
    }
}

FEN UIController::get_fen() {
    FEN fen;
    std::cout << "Paste FEN of your position:\n";
    std::cin >> fen.position >> fen.move >> fen.castles >> fen.en_passant_square >> fen.last_move;
    return fen;
}

void UIController::show_board(const Board &board, Color color) {
    if (color == Color::WHITE) {
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) {
                std::cout << board[i][j]._color << board[i][j]._type << " ";
            }
            std::cout << std::endl;
        }
    } else {
        for (int i = 0; i < 8; i++) {
            for (int j = 7; j >= 0; j--) {
                std::cout << board[i][j]._color << board[i][j]._type << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

Cell get_cell() {
    bool flag = false;
    std::string str;
    std::string str_raw;
    int i = 0;
    while (!flag) {
        str = "";
        if (i != 0) std::cout << "Wrong format, try again" << std::endl;
        i++;
        getline(std::cin, str_raw);
        for (char j : str_raw) {
            if (j != ' ') str += j;
        }
        if (str.size() == 2 && str[0] >= 'a' && str[0] <= 'h' && str[1] >= '1' && str[1] <= '8')
            flag = true;
    }

    std::cout << std::endl;

    return {str[0] - 'a', str[1] - '1'};
}

Move UIController::get_users_move() {
    Move ans;
    std::string str;
    std::string str_raw;

    std::cout << "From-square:" << std::endl;
    ans.from = get_cell();

    std::cout << "To-square:" << std::endl;
    ans.to = get_cell();

    return ans;
}

Type UIController::promote_to() {
    bool flag = false;
    std::string str;
    std::string str_raw;
    int i = 0;
    std::cout << "Promote to:" << std::endl;
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

    Type promote_to = Type::EMPTY;
    if (str == "N" || str == "n") promote_to = Type::KNIGHT;
    if (str == "B" || str == "b") promote_to = Type::BISHOP;
    if (str == "R" || str == "r") promote_to = Type::ROOK;
    if (str == "Q" || str == "q") promote_to = Type::QUEEN;
    return promote_to;
}

void UIController::impossible_move() {
    std::cout << "Impossible move, try again" << std::endl;
}

void UIController::finish_game(const Board &board, Color player, Result result, Color winner) {
    UIController::show_board(board, player);
    switch (result) {
        case Result::CHECKMATE_WIN:
            std::cout << "Mate. " << to_string(winner) << " won!" << std::endl;
            break;
        case Result::STALEMATE_DRAW:
            std::cout << "Stalemate. Draw!" << std::endl;
            break;
        case Result::FIFTY_MOVE_DRAW:
            std::cout << "50-move rule. Draw!" << std::endl;
            break;
        default:
            assert("Unknown result");
    }
}
