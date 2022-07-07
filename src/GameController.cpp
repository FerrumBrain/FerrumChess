#include "../include/GameController.h"
#include <sstream>
#include <cassert>

static std::pair<Type, Color> char_to_type_and_color(char c) {
    std::pair<Type, Color> ans = {Type::EMPTY, isupper(c) == 0 ? Color::BLACK : Color::WHITE};
    c = tolower(c);
    switch (c) {
        case 'r':
            ans.first = Type::ROOK;
            break;
        case 'n':
            ans.first = Type::KNIGHT;
            break;
        case 'b':
            ans.first = Type::BISHOP;
            break;
        case 'k':
            ans.first = Type::KING;
            break;
        case 'q':
            ans.first = Type::QUEEN;
            break;
        case 'p':
            ans.first = Type::PAWN;
            break;
        default:
            assert("Unknown char in FEN!");
    }

    return ans;
}

std::pair<std::pair<int, int>, std::pair<int, int>> GameController::handle_from_position() {
    std::pair<std::pair<int, int>, std::pair<int, int>> ans;

    std::string position, move, castles, en_passant_square;
    std::cout << "Paste FEN of your position (without last two fields): ";
    std::cin >> position >> move >> castles >> en_passant_square;

    board = Board(8, std::vector<Figure>(8, Figure(NONE)));
    int row = 7, column = 0;
    for (char c : position) {
        if (c == '/') {
            column = 0;
            row--;
        } else {
            if (isdigit(c) != 0) {
                column += static_cast<int>(c - '0');
            } else {
                auto[type, color] = char_to_type_and_color(c);
                bool is_moved = true;
                if (type == Type::KING) {
                    if (color == Color::WHITE) {
                        ans.first = {column, row};
                        if (isupper(castles[0]) != 0) {
                            is_moved = false;
                        }
                    } else {
                        ans.second = {column, row};
                        if (std::any_of(castles.begin(), castles.end(), [](char c) {
                            return islower(c) != 0;
                        })) {
                            is_moved = false;
                        }
                    }
                }

                if (column == 0 && row == 0) {
                    if (std::any_of(castles.begin(), castles.end(), [](char c) {
                        return c == 'Q';
                    })) {
                        is_moved = false;
                    }
                } else if (column == 7 && row == 0) {
                    if (std::any_of(castles.begin(), castles.end(), [](char c) {
                        return c == 'K';
                    })) {
                        is_moved = false;
                    }
                } else if (column == 0 && row == 7) {
                    if (std::any_of(castles.begin(), castles.end(), [](char c) {
                        return c == 'q';
                    })) {
                        is_moved = false;
                    }
                } else if (column == 7 && row == 7) {
                    if (std::any_of(castles.begin(), castles.end(), [](char c) {
                        return c == 'k';
                    })) {
                        is_moved = false;
                    }
                }

                board[row][column] = Figure(type, color, column, row, is_moved);
                column++;
            }
        }
    }

    is_white_first_to_move = (move == "w");

    history.clear();
    if (en_passant_square != "-") {
        int x = static_cast<int>(en_passant_square[0] - 'a'), y = static_cast<int>(en_passant_square[1] - '0');
        history.emplace_back(std::make_pair(x, y - (is_white_first_to_move ? -1 : 1)),
                             std::make_pair(x, y + (is_white_first_to_move ? -1 : 1)));
    }

    return ans;
}

GameController::GameController()
        : ui(Color::EMPTY, {-1, -1}), ai(Color::EMPTY, {-1, -1}) {
    std::string raw_input, input;
    bool from_position;
    std::pair<int, int> white_king_position, black_king_position;
    board = Board(8, std::vector<Figure>(8, Figure(NONE)));
    std::cout
            << R"(If you want to play new game, type "N". If you want to play from your own position, type "P")"
            << std::endl;
    while (true) {
        input = "";
        getline(std::cin, raw_input);
        for (char c: raw_input) {
            if (isspace(c) != 0) continue;
            input += c;
        }
        if (input == "N") {
            from_position = false;
            break;
        }
        if (input == "P") {
            std::tie(white_king_position, black_king_position) = handle_from_position(); // TODO: Incorrect FEN, move counter
            from_position = true;
            break;
        }
    }

    std::cout << R"(If you want to play with white pieces, type "W". Else type "B")" << std::endl;
    while (true) {
        input = "";
        getline(std::cin, raw_input);
        for (char c: raw_input) {
            if (c == ' ') continue;
            input += c;
        }
        if (input == "W") {
            user_color = Color::WHITE;
            break;
        }
        if (input == "B") {
            user_color = Color::BLACK;
            break;
        }
    }
    if (from_position) {
        ui = UserIntelligence(user_color, (user_color == Color::BLACK) ? black_king_position : white_king_position);
        ai = ArtificialIntelligence((user_color == Color::WHITE) ? Color::BLACK : Color::WHITE,
                                    (user_color == Color::WHITE) ? black_king_position : white_king_position);
    } else {
        board = Board(8, std::vector<Figure>(8, Figure(NONE)));
        is_white_first_to_move = true;
        int king_y = (user_color == Color::WHITE) ? 0 : 7;
        ui = UserIntelligence(user_color, {4, king_y});
        ai = ArtificialIntelligence((user_color == Color::WHITE) ? Color::BLACK : Color::WHITE, {4, 7 - king_y});
        history = {};
        for (int i = 0; i < 8; i++) {
            board[1][i] = Figure(Type::PAWN, Color::WHITE, i, 1);
            board[6][i] = Figure(Type::PAWN, Color::BLACK, i, 6);
            if (i == 0 || i == 7) {
                board[0][i] = Figure(Type::ROOK, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::ROOK, Color::BLACK, i, 7);
            } else if (i == 1 || i == 6) {
                board[0][i] = Figure(Type::KNIGHT, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::KNIGHT, Color::BLACK, i, 7);
            } else if (i == 2 || i == 5) {
                board[0][i] = Figure(Type::BISHOP, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::BISHOP, Color::BLACK, i, 7);
            } else if (i == 3) {
                board[0][i] = Figure(Type::QUEEN, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::QUEEN, Color::BLACK, i, 7);
            } else {
                board[0][i] = Figure(Type::KING, Color::WHITE, i, 0);
                board[7][i] = Figure(Type::KING, Color::BLACK, i, 7);
            }
        }
    }
}

void GameController::print_board() {
    if (user_color == Color::WHITE) {
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) {
                std::cout << board[i][j]._color << board[i][j]._type << " ";
            }
            std::cout << std::endl;
        }
    } else {
        for (int i = 0; i < 8; i++) {
            for (int j = 7; j >= 0; j--) {
                std::cout << board[i][j]._color << board[i][j]._type << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

bool GameController::is_mate(const Intelligence &intelligence) {
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> possible_moves;
    std::vector<std::pair<int, int>> current_moves;
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
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> possible_moves;
    std::vector<std::pair<int, int>> current_moves;
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
    while (true) {
        i++;
        if (!(is_white_first_to_move ^ (user_color == Color::WHITE))) {
            print_board();
            ui.make_move(board, history);
            if (is_mate(ai)) {
                print_board();
                std::cout << "Mate. You won!" << std::endl;
                return;
            }
            if (is_stalemate(ai)) {
                print_board();
                std::cout << "Stalemate. Draw!" << std::endl;
                return;
            }
            ai.make_move(board, history);
            if (is_mate(ui)) {
                print_board();
                std::cout << "Mate. You lost!" << std::endl;
                return;
            }
            if (is_stalemate(ui)) {
                print_board();
                std::cout << "Stalemate. Draw!" << std::endl;
                return;
            }
        } else {
            ai.make_move(board, history);
            if (is_mate(ui)) {
                print_board();
                std::cout << "Mate. You lost!" << std::endl;
                return;
            }
            if (is_stalemate(ui)) {
                print_board();
                std::cout << "Stalemate. Draw!" << std::endl;
                return;
            }
            print_board();
            ui.make_move(board, history);
            if (is_mate(ai)) {
                print_board();
                std::cout << "Mate. You won!" << std::endl;
                return;
            }
            if (is_stalemate(ai)) {
                print_board();
                std::cout << "Stalemate. Draw!" << std::endl;
                return;
            }
        }
    }
}