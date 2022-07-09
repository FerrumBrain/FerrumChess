#include "UserIntelligence.h"
#include "ArtificialIntelligence.h"

class GameController {
public:
    GameController();
    void play_game();
private:
    Board board;
    History history;
    UserIntelligence ui{Color::EMPTY, {-1, -1}};
    ArtificialIntelligence ai{Color::EMPTY, {-1, -1}};
    Color user_color;
    bool is_white_first_to_move;
    [[nodiscard]] bool is_mate(const Intelligence &intelligence) const;
    [[nodiscard]] bool is_stalemate(const Intelligence &intelligence) const;
    [[nodiscard]] bool is_finished(const Intelligence &intelligence) const;
    std::pair<Cell, Cell> handle_from_position();
    int last_move_for_50move = 0;
};