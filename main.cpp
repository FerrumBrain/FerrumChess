#include "UserIntelligence.h"
#include "ArtificialIntelligence.h"
#include <ctime>

using namespace std;

void print_board(const Board& board){
    for(int i = 7 ; i >= 0; i--){
        for(int j = 0; j < 8; j++){
            cout << board[i][j]._color << board[i][j]._type << " ";
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    srand(time(NULL));
    vector<vector<Figure>> board(8, vector<Figure>(8, Figure(NONE)));

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

    UserIntelligence ui(Color::WHITE, {4, 0});
    ArtificialIntelligence ai(Color::BLACK, {4, 7});
    History history = {};
    print_board(board);
    for(int u = 0; u < 5; u++){
        ui.make_move(board, history);
        ai.make_move(board, history);
        print_board(board);
    }
    for(auto move : history){
        cout << (char)('a' + move.first.first) << 1 + move.first.second << " -> " << (char)('a' + move.second.first) << 1 + move.second.second << endl;
    }
}
