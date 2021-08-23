#include "UserIntelligence.h"
#include "ArtificialIntelligence.h"

class GameController {
public:
    Board board;
    History history;
    UserIntelligence ui;
    ArtificialIntelligence ai;
    Color user_color;
    GameController();
    void play_game();
    // void end_game();
    bool is_mate(const Intelligence &intelligence);
    bool is_stalemate(const Intelligence &intelligence);
    void print_board();
};