#include "UserIntelligence.h"
#include "ArtificialIntelligence.h"

class GameController {
public:
    GameController();
    void play_game();
private:
    Board board;
    History history;
    UserIntelligence ui;
    ArtificialIntelligence ai;
    Color user_color;
    bool is_white_first_to_move;
    bool is_mate(const Intelligence &intelligence);
    bool is_stalemate(const Intelligence &intelligence);
    bool is_finished(const Intelligence &intelligence);
    std::pair<Cell, Cell> handle_from_position();
    int last_move_for_50move = 0;
};