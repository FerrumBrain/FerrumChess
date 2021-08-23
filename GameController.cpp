#include "GameController.h"

GameController::GameController() : ui(Color::EMPTY, {-1, -1}), ai(Color::EMPTY, {-1, -1}) {
    user_color = Color::WHITE;
    ui = UserIntelligence(user_color, {4, 0});
    ai = ArtificialIntelligence((user_color == Color::WHITE) ? Color::BLACK : Color::WHITE, {4, 7});
    history = {};
    board = Board(8, vector<Figure>(8, Figure(NONE)));
    for(int i = 0; i < 8; i++){
        board[1][i] = Figure(Type::PAWN, Color::WHITE, i, 1);
        board[6][i] = Figure(Type::PAWN, Color::BLACK, i, 6);
        if(i == 0 || i == 7) {
            board[0][i] = Figure(Type::ROOK, Color::WHITE, i, 0);
            board[7][i] = Figure(Type::ROOK, Color::BLACK, i, 7);
        } else if(i == 1 || i == 6) {
            board[0][i] = Figure(Type::KNIGHT, Color::WHITE, i, 0);
            board[7][i] = Figure(Type::KNIGHT, Color::BLACK, i, 7);
        } else if(i == 2 || i == 5) {
            board[0][i] = Figure(Type::BISHOP, Color::WHITE, i, 0);
            board[7][i] = Figure(Type::BISHOP, Color::BLACK, i, 7);
        } else if(i == 3) {
            board[0][i] = Figure(Type::QUEEN, Color::WHITE, i, 0);
            board[7][i] = Figure(Type::QUEEN, Color::BLACK, i, 7);
        } else {
            board[0][i] = Figure(Type::KING, Color::WHITE, i, 0);
            board[7][i] = Figure(Type::KING, Color::BLACK, i, 7);
        }
    }
}

void GameController::print_board() {
    if(user_color == Color::WHITE){
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
    while(true){
        i++;
        print_board();
        if(user_color == Color::WHITE){
            ui.make_move(board, history);
            if(is_mate(ai)){
                print_board();
                cout << "Mate. You won!" << endl;
                return;
            }
            if(is_stalemate(ai)){
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
            ai.make_move(board, history);
            if(is_mate(ui)){
                print_board();
                cout << "Mate. You lost!" << endl;
                return;
            }
            if(is_stalemate(ui)){
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
        } else {
            ai.make_move(board, history);
            if(is_mate(ui)){
                print_board();
                cout << "Mate. You lost!" << endl;
                return;
            }
            if(is_stalemate(ui)){
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
            ui.make_move(board, history);
            if(is_mate(ai)){
                print_board();
                cout << "Mate. You won!" << endl;
                return;
            }
            if(is_stalemate(ai)){
                print_board();
                cout << "Stalemate. Draw!" << endl;
                return;
            }
        }
    }
}