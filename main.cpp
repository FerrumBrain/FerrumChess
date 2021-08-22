#include "UserIntelligence.h"

using namespace std;

int main() {
    vector<vector<Figure>> board(8, vector<Figure>(8, Figure(NONE)));

    int x = 1, y = 1;
    Figure figure = Figure(Type::PAWN, Color::WHITE, x, y);
    board[y][x] = figure;
    board[0][0] = Figure(Type::QUEEN, Color::WHITE, 0, 0);
    board[2][2] = Figure(Type::KING, Color::WHITE, 2, 2);
    UserIntelligence ui(Color::WHITE, {2, 2});
    History history = {};
    for(int u = 0; u < 10; u++){
        ui.make_move(board, history);
        for(int i = 7 ; i >= 0; i--){
            for(int j = 0; j < 8; j++){
                cout << board[i][j]._type << " ";
            }
            cout << endl;
        }
    }
}
