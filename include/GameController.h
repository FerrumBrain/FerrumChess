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
    void print_board();
    std::pair<std::pair<int, int>, std::pair<int, int>> handle_from_position();
};