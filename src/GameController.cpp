#include "../include/GameController.h"
#include "../include/UIController.h"
#include <cassert>
#include <chrono>

int GameController::last_move_for_50move = 0;
Board GameController::board = {};
History GameController::history = {};

static std::pair<Type, Color> char_to_type_and_color(char c) {
    std::pair<Type, Color> ans = {Type::EMPTY, isupper(c) == 0 ? Color::BLACK : Color::WHITE};
    c = static_cast<char>(tolower(c));
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

std::pair<Cell, Cell> GameController::handle_from_position() {
    std::pair<Cell, Cell> ans;

    FEN fen = UIController::get_fen();

    last_move_for_50move = fen.last_move * -2;

    board = Board(8, std::vector<Figure>(8, Figure(NONE)));
    int row = 7;
    int column = 0;
    for (char c : fen.position) {
        if (c == '/') {
            column = 0;
            row--;
            continue;
        }
        if (isdigit(c) != 0) {
            column += c - '0';
            continue;
        }
        auto[type, color] = char_to_type_and_color(c);

        if (type == Type::KING && color == Color::WHITE) {
            ans.first = {column, row};
        }
        if (type == Type::KING && color == Color::BLACK) {
            ans.second = {column, row};
        }

        bool is_moved = !(
                (type == Type::KING && color == Color::WHITE && isupper(fen.castles[0]) != 0) ||
                (type == Type::KING && color == Color::BLACK &&
                 (std::ranges::any_of(fen.castles.begin(), fen.castles.end(), [](char ch) {
                     return islower(ch) != 0;
                 }))) ||
                (column == 0 && row == 0 &&
                 std::ranges::any_of(fen.castles.begin(), fen.castles.end(), [](char ch) {
                     return ch == 'Q';
                 })) ||
                (column == 7 && row == 0 &&
                 std::ranges::any_of(fen.castles.begin(), fen.castles.end(), [](char ch) {
                     return ch == 'K';
                 })) ||
                (column == 0 && row == 7 &&
                 std::ranges::any_of(fen.castles.begin(), fen.castles.end(), [](char ch) {
                     return ch == 'q';
                 })) ||
                (column == 7 && row == 7 &&
                 std::ranges::any_of(fen.castles.begin(), fen.castles.end(), [](char ch) {
                     return ch == 'k';
                 }))
        );

        board[row][column] = Figure(type, color, column, row, is_moved);
        column++;
    }

    is_white_first_to_move = (fen.move == "w");

    history.clear();
    if (fen.en_passant_square != "-") {
        int x = fen.en_passant_square[0] - 'a';
        int y = fen.en_passant_square[1] - '0';
        history.emplace_back(Cell(x, y - (is_white_first_to_move ? -1 : 1)),
                             Cell(x, y + (is_white_first_to_move ? -1 : 1)));
    }

    return ans;
}

GameController::GameController() {
    bool from_position;
    Cell white_king_position;
    Cell black_king_position;
    board = Board(8, std::vector<Figure>(8, Figure(NONE)));

    if (UIController::start_game() == Mode::NEW_GAME) {
        from_position = false;
    } else {
        std::tie(white_king_position, black_king_position) = handle_from_position(); // TODO: Incorrect FEN
        from_position = true;
    }

    user_color = UIController::choose_color();

    if (from_position) {
        ui = UserIntelligence(user_color, (user_color == Color::BLACK) ? black_king_position : white_king_position);
        ai = ArtificialIntelligence((user_color == Color::WHITE) ? Color::BLACK : Color::WHITE,
                                    (user_color == Color::WHITE) ? black_king_position : white_king_position);
        return;
    }

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

std::vector<Move> get_possible_moves(Color color, Cell king_position, const Board &board, const History &history) {
    std::vector<Move> possible_moves;
    std::vector<Cell> current_moves;
    int i = 0;
    int j = 0;
    while (i < 8) {
        if (board[i][j]._color == color) {
            current_moves = Intelligence::controller_container.at(board[i][j]._type)->get_moves({j, i}, board,
                                                                                                history.empty()
                                                                                                ? Move()
                                                                                                : history.back(),
                                                                                                king_position);
            for (auto to : current_moves) {
                possible_moves.push_back({{j, i}, to});
            }
        }
        j++;
        if (j == 8) {
            j = 0;
            i++;
        }
    }

    return possible_moves;
}

bool GameController::is_mate(Color color, Cell king_position) {
    auto possible_moves = get_possible_moves(color, king_position, board, history);
    Color opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return possible_moves.empty() && KingController::is_attacked(king_position, opposite_color, board);
}


bool GameController::is_stalemate(Color color, Cell king_position) {
    auto possible_moves = get_possible_moves(color, king_position, board, history);
    Color opposite_color = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return possible_moves.empty() && !KingController::is_attacked(king_position, opposite_color, board);
}

bool GameController::is_50_move() {
    return history.size() - last_move_for_50move == 100;
}

bool GameController::is_finished(const Intelligence &intelligence) const {
    if (is_mate(intelligence._color, intelligence._king_position)) {
        UIController::finish_game(board, user_color, Result::CHECKMATE_WIN, opposite(intelligence._color));
        return true;
    }

    if (is_stalemate(intelligence._color, intelligence._king_position)) {
        UIController::finish_game(board, user_color, Result::STALEMATE_DRAW);
        return true;
    }

    if (is_50_move()) {
        UIController::finish_game(board, user_color, Result::FIFTY_MOVE_DRAW);
        return true;
    }
    return false;
}

void GameController::play_game() {
    std::vector<Intelligence *> players;
    if (is_white_first_to_move && user_color == Color::WHITE || !is_white_first_to_move && user_color == Color::BLACK) {
        players = {&ui, &ai};
    } else {
        players = {&ai, &ui};
    }

    int i = 0;
    while (true) {
        if (players[i] == &ui) {
            UIController::show_board(board, user_color);
        }
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        history.emplace_back(
                players[i]->make_move(board, history.empty() ? Move() : history.back()));
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
                  << "[ns]" << std::endl;
        std::cout << "Last move was: " << (char) (history.back().from.x + 'a') << history.back().from.y + 1
                  << " -> " << (char) (history.back().to.x + 'a') << history.back().to.y + 1 << std::endl;


        if (board[history.back().from.y][history.back().from.x]._type == Type::PAWN ||
            board[history.back().to.y][history.back().to.x]._type != Type::EMPTY) {
            last_move_for_50move = static_cast<int>(history.size()) + 1;
        }

        if (is_finished(*players[1 - i])) return;
        i = 1 - i;
    }
}