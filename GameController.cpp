#include "GameController.h"
#include <fstream>

GameController::GameController()
    : ui(Color::EMPTY, {-1, -1})
    , ai(Color::EMPTY, {-1, -1})
{
    ifstream saved_position("position.txt");
    string raw_input, input;
    bool from_position;
    pair<int, int> white_king_position, black_king_position;
    board = Board(8, vector<Figure>(8, Figure(NONE)));
    if(saved_position.is_open()) {
        cout << "You have saved position:" << endl;
        char saved_color;
        saved_position >> saved_color;
        if(saved_color == 'W') cout  << "White to move" << endl;
        else cout  << "Black to move" << endl;
        is_white_first_to_move = (saved_color == 'W');
        string cur;
        vector<vector<bool>> is_moved(8, vector<bool>(8));
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                int a;
                saved_position >> a;
                is_moved[7 - i][j] = (a == 1);
            }
        }

        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                saved_position >> cur;
                cout << cur << " ";

                Type cur_type = Type::EMPTY;
                if(cur[1] == 'P') cur_type = Type::PAWN;
                if(cur[1] == 'N') cur_type = Type::KNIGHT;
                if(cur[1] == 'B') cur_type = Type::BISHOP;
                if(cur[1] == 'R') cur_type = Type::ROOK;
                if(cur[1] == 'Q') cur_type = Type::QUEEN;
                if(cur[1] == 'K') cur_type = Type::KING;

                if(cur[0] == 'W') {
                    board[7 - i][j] = Figure(cur_type, Color::WHITE, j, 7 - i, is_moved[7 - i][j]);
                    if(cur[1] == 'K') white_king_position = {j, 7 - i};
                } else if(cur[0] == 'B') {
                    board[7 - i][j] = Figure(cur_type, Color::BLACK, j, 7 - i, is_moved[7 - i][j]);
                    if(cur[1] == 'K') black_king_position = {j, 7 - i};
                } else {
                    board[7 - i][j] = NONE;
                }
            }
            cout << endl;
        }

        int x_from, y_from, x_to, y_to;
        saved_position >> x_from >> y_from >> x_to >> y_to;
        history = {{{x_from, y_from}, {x_to, y_to}}};
        cout << "Last move was: " << (char)('a' + x_from) << y_from + 1 << " -> " << (char)('a' + x_to) << y_to + 1 << endl;

        cout
            << R"(If you want to play new game, type "N". If you want to play from saved position, type "S")"
            << endl
        ;
        while (true) {
            input = "";
            getline(cin, raw_input);
            for (char c: raw_input) {
                if (c == ' ') continue;
                input += c;
            }
            if (input == "N") {
                from_position = false;
                break;
            }
            if (input == "S") {
                from_position = true;
                break;
            }
        }
    }
    saved_position.close();
    cout << R"(If you want to play with white pieces, type "W". Else type "B")" << endl;
    while (true) {
        input = "";
        getline(cin, raw_input);
        for (char c: raw_input) {
            if (c == ' ') continue;
            input += c;
        }
        if (input == "W") {
            user_color = Color::WHITE;
            break;
        }
        if (input == "B") {
            user_color = Color::BLACK;
            break;
        }
    }
    if(from_position){
        ui = UserIntelligence(user_color, (user_color == Color::BLACK) ? black_king_position : white_king_position);
        ai = ArtificialIntelligence((user_color == Color::WHITE) ? Color::BLACK : Color::WHITE,
                                    (user_color == Color::WHITE) ? black_king_position : white_king_position);
    } else {
        board = Board(8, vector<Figure>(8, Figure(NONE)));
        is_white_first_to_move = true;
        int king_y = (user_color == Color::WHITE) ? 0 : 7;
        ui = UserIntelligence(user_color, {4, king_y});
        ai = ArtificialIntelligence((user_color == Color::WHITE) ? Color::BLACK : Color::WHITE, {4, 7 - king_y});
        history = {};
        for (int i = 0; i < 8; i++) {
            board[1][i] = Figure(Type::PAWN, Color::WHITE, i, 1);
            board[6][i] = Figure(Type::PAWN, Color::BLACK, i, 6);
            if (i == 0 || i == 7) {
                board[0][i] = Figure(Type::ROOK, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::ROOK, Color::BLACK, i, 7);
            } else if (i == 1 || i == 6) {
                board[0][i] = Figure(Type::KNIGHT, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::KNIGHT, Color::BLACK, i, 7);
            } else if (i == 2 || i == 5) {
                board[0][i] = Figure(Type::BISHOP, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::BISHOP, Color::BLACK, i, 7);
            } else if (i == 3) {
                board[0][i] = Figure(Type::QUEEN, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::QUEEN, Color::BLACK, i, 7);
            } else {
                board[0][i] = Figure(Type::KING, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::KING, Color::BLACK, i, 7);
            }
        }
    }
}

void GameController::print_board() {
    if (user_color == Color::WHITE) {
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) {
                cout << board[i][j]._color << board[i][j]._type << " ";
            }
            cout << endl;
        }
    } else {
        for (int i = 0; i < 8; i++) {
            for (int j = 7; j >= 0; j--) {
                cout << board[i][j]._color << board[i][j]._type << " ";
            }
            cout << endl;
        }
    }
    cout << endl;
}

bool GameController::is_mate(const Intelligence &intelligence) {
    vector<pair<pair<int, int>, pair<int, int>>> possible_moves;
    vector<pair<int, int>> current_moves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]._color == intelligence._color) {
                current_moves = intelligence.controller_container.at(board[i][j]._type)->get_moves({j, i}, board,
                                                                                                   history,
                                                                                                   intelligence._king_position);
                for (auto to : current_moves) {
                    possible_moves.push_back({{j, i}, to});
                }
            }
        }
    }
    KingController kingController;
    Color opposite_color = (intelligence._color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return possible_moves.empty() && kingController.is_attacked(intelligence._king_position, opposite_color, board);
}

bool GameController::is_stalemate(const Intelligence &intelligence) {
    vector<pair<pair<int, int>, pair<int, int>>> possible_moves;
    vector<pair<int, int>> current_moves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]._color == intelligence._color) {
                current_moves = intelligence.controller_container.at(board[i][j]._type)->get_moves({j, i}, board,
                                                                                                   history,
                                                                                                   intelligence._king_position);
                for (auto to : current_moves) {
                    possible_moves.push_back({{j, i}, to});
                }
            }
        }
    }
    KingController kingController;
    Color opposite_color = (intelligence._color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return possible_moves.empty() && !kingController.is_attacked(intelligence._king_position, opposite_color, board);
}

void GameController::play_game() {
    int i = 0;
    while (true) {
        i++;
        if (!(is_white_first_to_move ^ (user_color == Color::WHITE))) {
            print_board();
            ui.make_move(board, history);
            if (is_mate(ai)) {
                print_board();
                cout << "Mate. You won!" << endl;
                return;
            }
            if (is_stalemate(ai)) {
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
            ai.make_move(board, history);
            if (is_mate(ui)) {
                print_board();
                cout << "Mate. You lost!" << endl;
                return;
            }
            if (is_stalemate(ui)) {
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
        } else {
            ai.make_move(board, history);
            if (is_mate(ui)) {
                print_board();
                cout << "Mate. You lost!" << endl;
                return;
            }
            if (is_stalemate(ui)) {
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
            print_board();
            ui.make_move(board, history);
            if (is_mate(ai)) {
                print_board();
                cout << "Mate. You won!" << endl;
                return;
            }
            if (is_stalemate(ai)) {
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
        }
    }
}