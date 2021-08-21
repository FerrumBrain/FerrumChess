#include "PawnController.h"

using namespace std;

int main() {
    vector<vector<Figure>> board(8, vector<Figure>(8, Figure(NONE)));

    int x = 1, y = 6;
    Figure figure = Figure(Type::PAWN, Color::WHITE, x, y, true);
    board[y][x] = figure;
    board[7][0] = Figure(Type::KING, Color::WHITE, 0, 7);
    board[7][7] = Figure(Type::ROOK, Color::BLACK, 7, 7);

    PawnController controller;
    History history = {};
    vector<pair<int, int>> ans = controller.get_moves(figure._coords, board, history, {0, 7});
    sort(ans.begin(), ans.end());
    for (pair<int, int> i: ans) {
        cout << "FROM " << (char) ('a' + figure._coords.first) << figure._coords.second + 1 << " TO "
             << (char) ('a' + i.first) << i.second + 1 << endl;
    }

    for(int i = 7 ; i >= 0; i--){
        for(int j = 0; j < 8; j++){
            char cur = '-';
            if(board[i][j]._type == Type::PAWN) cur = 'P';
            if(board[i][j]._type == Type::KNIGHT) cur = 'N';
            if(board[i][j]._type == Type::BISHOP) cur = 'B';
            if(board[i][j]._type == Type::ROOK) cur = 'R';
            if(board[i][j]._type == Type::QUEEN) cur = 'Q';
            if(board[i][j]._type == Type::KING) cur = 'K';
            cout << cur << " ";
        }
        cout << endl;
    }
    cout << endl;

    controller.make_move(figure._coords, {1, 7}, board, history, Type::QUEEN);

    for(int i = 7 ; i >= 0; i--){
        for(int j = 0; j < 8; j++){
            char cur = '-';
            if(board[i][j]._type == Type::PAWN) cur = 'P';
            if(board[i][j]._type == Type::KNIGHT) cur = 'N';
            if(board[i][j]._type == Type::BISHOP) cur = 'B';
            if(board[i][j]._type == Type::ROOK) cur = 'R';
            if(board[i][j]._type == Type::QUEEN) cur = 'Q';
            if(board[i][j]._type == Type::KING) cur = 'K';
            cout << cur << " ";
        }
        cout << endl;
    }
}
