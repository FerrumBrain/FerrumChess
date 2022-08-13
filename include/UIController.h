#pragma once

#include "controllers/FigureController.h"

enum class Mode {
    NEW_GAME,
    FEN_GAME
};

struct FEN {
    std::string position;
    std::string move;
    std::string castles;
    std::string en_passant_square;
    int last_move;
};

enum class Result {
    CHECKMATE_WIN,
    //RESIGN_BLACK_WIN,
    //RESIGN_WHITE_WIN,
    STALEMATE_DRAW,
    FIFTY_MOVE_DRAW,
    POSITION_DRAW
};

namespace UIController {

    Mode start_game();

    Color choose_color();

    FEN get_fen();

    void show_board(const Board &board, Color color);

    Move get_users_move();

    Type promote_to();

    void impossible_move();

    void finish_game(const Board &board, Color player, Result result, Color winner = Color::EMPTY);

}