#pragma once

#include "UserIntelligence.h"
#include "ArtificialIntelligence.h"

class GameController {
public:
    GameController();
    void play_game();
    [[nodiscard]] static bool is_mate(Color color, Cell king_position);
    [[nodiscard]] static bool is_stalemate(Color color, Cell king_position);
    [[nodiscard]] static bool is_50_move();
    [[nodiscard]] static bool is_draw();
private:
    static Board board;
    static History history;
    UserIntelligence ui{Color::EMPTY, {-1, -1}};
    ArtificialIntelligence ai{Color::EMPTY, {-1, -1}};
    Color user_color;
    bool is_white_first_to_move;
    [[nodiscard]] bool is_finished(const Intelligence &intelligence) const;
    std::pair<Cell, Cell> handle_from_position();
    static int last_move_for_50move;
};