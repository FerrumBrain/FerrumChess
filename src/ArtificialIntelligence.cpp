#include "../include/ArtificialIntelligence.h"

#include <random>

void ArtificialIntelligence::fill_figures(const Board &board) {
    figures[0].clear();
    figures[1].clear();

    middlegame_piece_eval = {0, 0};
    endgame_piece_eval = {0, 0};
    gamephase = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]._color == Color::EMPTY) continue;

            int index = board[i][j]._color == _color ? 0 : 1;


            figures[index].emplace_back(board[i][j]);
            if ((_color == Color::BLACK && index == 0) || (index == 1 && _color == Color::WHITE)) {
                middlegame_piece_eval[index] += middlegame_piece_value[static_cast<int>(board[i][j]._type)] +
                                                middlegame_tables[static_cast<int>(board[i][j]._type)][7 - i][7 - j];
                endgame_piece_eval[index] += endgame_piece_value[static_cast<int>(board[i][j]._type)] +
                                             endgame_tables[static_cast<int>(board[i][j]._type)][7 - i][7 - j];
            } else {
                middlegame_piece_eval[index] += middlegame_piece_value[static_cast<int>(board[i][j]._type)] +
                                                middlegame_tables[static_cast<int>(board[i][j]._type)][i][j];
                endgame_piece_eval[index] += endgame_piece_value[static_cast<int>(board[i][j]._type)] +
                                             endgame_tables[static_cast<int>(board[i][j]._type)][i][j];
            }
            gamephase += gamephase_values[static_cast<int>(board[i][j]._type)];
        }
    }
}

static bool is_better(double eval1, double eval2) {
    return eval1 > eval2;
}

Move ArtificialIntelligence::make_move(Board &board, Move last_move) {
    Board board_copy = board;
    counter = 0;
    fill_figures(board);
    for (const auto &figure : figures[1]) {
        if (figure._type == Type::KING) {
            _opponent_king_position = figure._coords;
            break;
        }
    }
    hash_position(board);

    Move ans;
    Type promote_to;
    if (opening_table.contains(hash)) {
        std::discrete_distribution<int> distribution(opening_table[hash].second.begin(),
                                                     opening_table[hash].second.end());
        ans = opening_table[hash].first[distribution(gen)];
        promote_to = Type::EMPTY;
    } else {
        std::tie(ans, promote_to) = search(board_copy, last_move, 0, -BIG_INFINITY, BIG_INFINITY,
                                           Priority::NOTHING).second;
    }

    auto[from, to] = ans;
    Type type = board[from.y][from.x]._type;
    controller_container[type]->make_move(ans, board, last_move, promote_to);
    if (board[to.y][to.x]._type == Type::KING)
        _king_position = to;

    std::cout << "Positions evaluated: " << counter << std::endl;

    return ans;
}

static bool is_insufficient(const std::vector<Figure> &figures) {
    return figures.size() == 1 || (figures.size() == 2 &&
                                   (figures[0]._type == Type::KNIGHT || figures[0]._type == Type::BISHOP ||
                                    figures[1]._type == Type::KNIGHT || figures[1]._type == Type::BISHOP));
}

void ArtificialIntelligence::undo_move(Board &board, Move last_move, const Figure &old_figure_from,
                                       const Figure &old_figure_to, int player, int i, int index_in_figures) {
    history[this->hash]--;
    change_eval(player, -1, i, last_move.to);
    hash ^= zobrist_table[8 * last_move.to.y + last_move.to.x][2 * static_cast<int>(figures[player][i]._type) +
                                                               static_cast<int>(figures[player][i]._color)];
    board[last_move.from.y][last_move.from.x] = old_figure_from;
    board[last_move.to.y][last_move.to.x] = old_figure_to;
    figures[player][i] = old_figure_from;
    change_eval(player, +1, i, last_move.from);
    hash ^= zobrist_table[8 * last_move.from.y + last_move.from.x][2 * static_cast<int>(figures[player][i]._type) +
                                                                   static_cast<int>(figures[player][i]._color)];
    if (index_in_figures != -1) {
        if (old_figure_to._type == Type::EMPTY && last_move.from.x - last_move.to.x != 0) {
            int delta = figures[player][i]._color == Color::WHITE ? -1 : 1;
            figures[1 - player][index_in_figures]._type = old_figure_from._type;
            figures[1 - player][index_in_figures]._color = opposite(figures[player][i]._color);
            figures[1 - player][index_in_figures]._coords = {last_move.to.x,
                                                             last_move.to.y + delta};
            board[last_move.to.y + delta][last_move.to.x] = figures[1 - player][index_in_figures];
            change_eval(1 - player, +1, index_in_figures, {last_move.to.x, last_move.to.y + delta});
            hash ^= zobrist_table[8 * (last_move.to.y + delta) + last_move.to.x][
                    2 * static_cast<int>(figures[1 - player][index_in_figures]._type) +
                    static_cast<int>(figures[1 - player][index_in_figures]._color)];
            gamephase += gamephase_values[static_cast<int>(figures[1 - player][index_in_figures]._type)];
        } else {
            figures[1 - player][index_in_figures]._type = old_figure_to._type;
            change_eval(1 - player, +1, index_in_figures, last_move.to);
            hash ^= zobrist_table[8 * last_move.to.y + last_move.to.x][
                    2 * static_cast<int>(figures[1 - player][index_in_figures]._type) +
                    static_cast<int>(figures[1 - player][index_in_figures]._color)];
            gamephase += gamephase_values[static_cast<int>(figures[1 - player][index_in_figures]._type)];
        }
    }
    if (figures[player][i]._type == Type::KING) {
        set_king_position(player, last_move.from);
        if (abs(last_move.from.x - last_move.to.x) == 2) {
            int rook_delta = last_move.from.x - last_move.to.x < 0 ? -1 : 1;
            int rook_start_pos = last_move.from.x - last_move.to.x < 0 ? 7 : 0;
            board[last_move.from.y][rook_start_pos] = board[last_move.to.y][last_move.to.x + rook_delta];
            board[last_move.from.y][rook_start_pos]._coords = {rook_start_pos, last_move.to.y};
            auto index = static_cast<int>(std::ranges::find_if(figures[player].begin(), figures[player].end(),
                                                               [&last_move, &rook_delta](const Figure &figure) {
                                                                   return figure._coords ==
                                                                          Cell{last_move.to.x + rook_delta,
                                                                               last_move.to.y};
                                                               }) - figures[player].begin());
            change_eval(player, -1, index, {last_move.to.x + rook_delta, last_move.to.y});
            hash ^= zobrist_table[8 * last_move.to.y + last_move.to.x + rook_delta][
                    2 * static_cast<int>(figures[player][index]._type) +
                    static_cast<int>(figures[player][index]._color)];
            figures[player][index]._coords = {rook_start_pos, last_move.to.y};
            change_eval(player, +1, index, {rook_start_pos, last_move.to.y});
            hash ^= zobrist_table[8 * last_move.to.y + rook_start_pos][
                    2 * static_cast<int>(figures[player][index]._type) +
                    static_cast<int>(figures[player][index]._color)];

            board[last_move.to.y][last_move.to.x + rook_delta] = NONE;
        }
    }
}

void ArtificialIntelligence::set_king_position(int player, Cell new_king_position) {
    if (player == 0) _king_position = new_king_position;
    else _opponent_king_position = new_king_position;
}

std::pair<int, std::pair<Move, Type>>
ArtificialIntelligence::search(Board &board, Move last_move, int depth, int alpha, int beta,
                               Priority last_move_priority) {
    int player = depth % 2;
    Move ans;
    Type promote_to = Type::EMPTY;

    if (trasposition_table.contains({player, hash, depth, alpha, beta})) {
        return trasposition_table.at({player, hash, depth, alpha, beta});
    }

    if (Cell king = player == 1 ? _opponent_king_position : _king_position; board[king.y][king.x]._type !=
                                                                            Type::KING) {
        return {(DEPTH - depth + 1) * -INF, {Move(), Type::EMPTY}};
    }

    auto possible_moves = get_possible_moves(player, last_move, board, last_move_priority != Priority::CHECK);

    if (possible_moves.empty()) {
        if(last_move_priority == Priority::CHECK) {
            return {(DEPTH - depth + 1) * -INF, {Move(), Type::EMPTY}};
        }
        return {0, {Move(), Type::EMPTY}};
    }

    if (is_insufficient(figures[0]) && is_insufficient(figures[1]) || history[hash] == 3) {
        return {0, {Move(), Type::EMPTY}};
    }

    if (depth == DEPTH)
        return {evaluate(player), {Move(), Type::EMPTY}};

    for (auto const &[priority, number_move_promotion] : possible_moves) {
        auto const &[i, move_and_promote_to] = number_move_promotion;
        auto move = move_and_promote_to.first;
        auto possible_promote_to = move_and_promote_to.second;
        Cell old_coords = figures[player][i]._coords;
        auto old_figure_from = figures[player][i];
        Figure old_figure_to = board[move.to.y][move.to.x];
        // -------------------------------------------------------------------------------------------------------------
        int index_in_figures = make_pseudo_move(player, i, move, old_figure_to, board, last_move, possible_promote_to);
#ifdef DEBUG
        stack.emplace_back(move);
#endif
        // -------------------------------------------------------------------------------------------------------------
        int cur_eval = -search(board, {old_coords, move.to}, depth + 1, -beta, -alpha, priority).first;
        // -------------------------------------------------------------------------------------------------------------
        undo_move(board, {old_coords, move.to}, old_figure_from, old_figure_to, player, i, index_in_figures);
#ifdef DEBUG
        stack.pop_back();
#endif
        // -------------------------------------------------------------------------------------------------------------
        if (cur_eval >= beta) {
            trasposition_table[{player, hash, depth, alpha, beta}] = {beta, {Move(), Type::EMPTY}};
            return {beta, {Move(), Type::EMPTY}};
        }

        if (is_better(cur_eval, alpha)) {
            alpha = cur_eval;
            ans = {old_coords, move.to};
            promote_to = possible_promote_to;
        }
    }

    trasposition_table[{player, hash, depth, alpha, beta}] = {alpha, {ans, promote_to}};

    return {alpha, {ans, promote_to}};
}

ArtificialIntelligence::Priority
ArtificialIntelligence::priority(int player, Move last_move, const std::pair<int, std::pair<Move, Type>> &a,
                                 Board &board) {
    auto is_attack = [this, &board, &player, &last_move](Move move, Type promote_to, int i) {
        Cell old_coords = move.from;
        auto old_figure_from = board[move.from.y][move.from.x];
        Figure old_figure_to = board[move.to.y][move.to.x];
        int index_in_figures = make_pseudo_move(player, i, move, old_figure_to, board, last_move, promote_to);

        auto possible_cells = controller_container[old_figure_from._type]->get_moves(move.to, board, move,
                                                                                     player == 0 ? _king_position
                                                                                                 : _opponent_king_position,
                                                                                     false);
        bool res = std::ranges::any_of(possible_cells.begin(), possible_cells.end(), [&board](Cell c) {
            return board[c.y][c.x]._type != Type::EMPTY;
        });

        undo_move(board, {old_coords, move.to}, old_figure_from, old_figure_to, player, i, index_in_figures);
        return res;
    };
    auto is_capture = [&board](Move move, Type, int) {
        return board[move.to.y][move.to.x] != NONE;
    };
    auto is_check = [this, &board, &player, &last_move](Move move, Type promote_to, int i) {
        Cell old_coords = move.from;
        auto old_figure_from = board[move.from.y][move.from.x];
        Figure old_figure_to = board[move.to.y][move.to.x];
        int index_in_figures = make_pseudo_move(player, i, move, old_figure_to, board, last_move, promote_to);
        bool res = KingController::is_attacked(player == 0 ? _opponent_king_position : _king_position,
                                               player == 0 ? _color : opposite(_color), board);
        undo_move(board, {old_coords, move.to}, old_figure_from, old_figure_to, player, i, index_in_figures);
        return res;
    };

    Move move = a.second.first;
    Type promote_to = a.second.second;

    Priority priority = Priority::NOTHING;

    if (is_check(move, promote_to, a.first)) priority = Priority::CHECK;

    if (is_capture(move, promote_to, a.first)) {
        if (board[move.to.y][move.to.x]._type == Type::KING) {
            priority = Priority::KING_TAKE;
        } else if (priority == Priority::NOTHING) {
            priority = Priority::CAPTURE;
        }
    }

    if (priority == Priority::NOTHING && is_attack(move, promote_to, a.first)) priority = Priority::ATTACK;

    return priority;
}

std::vector<std::pair<ArtificialIntelligence::Priority, std::pair<int, std::pair<Move, Type>>>>
ArtificialIntelligence::get_possible_moves(int player,
                                           Move last_move,
                                           Board &board, bool king_take) {
    std::vector<std::pair<Priority, std::pair<int, std::pair<Move, Type>>>> possible_moves;
    std::vector<Cell> current_cells;
    for (int i = 0; i < figures[player].size(); i++) {
        if (figures[player][i]._type == Type::EMPTY)
            continue;
        current_cells = controller_container[figures[player][i]._type]->get_moves(figures[player][i]._coords, board,
                                                                                  last_move,
                                                                                  player == 0 ? _king_position
                                                                                              : _opponent_king_position,
                                                                                  king_take);
        std::pair<int, std::pair<Move, Type>> cur;
        for (auto to : current_cells) {
            if (figures[player][i]._type == Type::PAWN && to.y % 7 == 0) {
                cur = {i, {{figures[player][i]._coords, to}, Type::QUEEN}};
                possible_moves.emplace_back(priority(player, last_move, cur, board), cur);
                cur.second.second = Type::ROOK;
                possible_moves.emplace_back(priority(player, last_move, cur, board), cur);
                cur.second.second = Type::BISHOP;
                possible_moves.emplace_back(priority(player, last_move, cur, board), cur);
                cur.second.second = Type::KNIGHT;
                possible_moves.emplace_back(priority(player, last_move, cur, board), cur);
            } else {
                cur = {i, {{figures[player][i]._coords, to}, Type::EMPTY}};
                possible_moves.emplace_back(priority(player, last_move, cur, board), cur);
            }
        }
    }

    std::ranges::sort(possible_moves.begin(), possible_moves.end(),
                      [](const std::pair<Priority, std::pair<int, std::pair<Move, Type>>> &a,
                         const std::pair<Priority, std::pair<int, std::pair<Move, Type>>> &b) {
                          return a.first > b.first;
                      });

    return possible_moves;
}


int ArtificialIntelligence::make_pseudo_move(int player, int i, Move move, const Figure &old_figure_to, Board &board,
                                             Move last_move, Type possible_promote_to) {
    change_eval(player, -1, i, move.from);
    hash ^= zobrist_table[8 * move.from.y + move.from.x][2 * static_cast<int>(figures[player][i]._type) +
                                                         static_cast<int>(figures[player][i]._color)];
    int index_in_figures = -1;
    if (old_figure_to != NONE) {
        auto iter = std::ranges::find_if(figures[1 - player].begin(), figures[1 - player].end(),
                                         [&](const Figure &figure) {
                                             return figure._coords == move.to && figure._type != Type::EMPTY;
                                         });
        index_in_figures = static_cast<int>(iter - figures[1 - player].begin());
        change_eval(1 - player, -1, index_in_figures, move.to);
        hash ^= zobrist_table[8 * move.to.y + move.to.x][
                2 * static_cast<int>(figures[1 - player][index_in_figures]._type) +
                static_cast<int>(figures[1 - player][index_in_figures]._color)];
        gamephase -= gamephase_values[static_cast<int>(figures[1 - player][index_in_figures]._type)];
        figures[1 - player][index_in_figures]._type = Type::EMPTY;
    } else if (figures[player][i]._type == Type::PAWN && move.to.x - move.from.x != 0) {
        int delta = figures[player][i]._color == Color::WHITE ? -1 : 1;
        auto iter = std::ranges::find_if(figures[1 - player].begin(), figures[1 - player].end(),
                                         [&](const Figure &figure) {
                                             return figure._coords == Cell{move.to.x, move.to.y + delta} &&
                                                    figure._type != Type::EMPTY;
                                         });
        index_in_figures = static_cast<int>(iter - figures[1 - player].begin());
        change_eval(1 - player, -1, index_in_figures, {move.to.x, move.to.y + delta});
        hash ^= zobrist_table[8 * (move.to.y + delta) + move.to.x][
                2 * static_cast<int>(figures[1 - player][index_in_figures]._type) +
                static_cast<int>(figures[1 - player][index_in_figures]._color)];
        gamephase -= gamephase_values[static_cast<int>(figures[1 - player][index_in_figures]._type)];
        figures[1 - player][index_in_figures]._type = Type::EMPTY;
    }
    controller_container[figures[player][i]._type]->make_move(
            {figures[player][i]._coords, move.to}, board, last_move, possible_promote_to);
    figures[player][i] = board[move.to.y][move.to.x];
    if (figures[player][i]._type == Type::KING) {
        set_king_position(player, move.to);
        if (abs(move.from.x - move.to.x) == 2) {
            int rook_delta = move.from.x - move.to.x < 0 ? -1 : 1;
            int rook_start_pos = move.from.x - move.to.x < 0 ? 7 : 0;
            auto index = static_cast<int>(std::ranges::find_if(figures[player].begin(), figures[player].end(),
                                                               [&move, &rook_start_pos](const Figure &figure) {
                                                                   return figure._coords ==
                                                                          Cell{rook_start_pos, move.to.y} &&
                                                                          figure._type != Type::EMPTY;
                                                               }) - figures[player].begin());
            change_eval(player, -1, index, {rook_start_pos, move.to.y});
            hash ^= zobrist_table[8 * move.to.y + rook_start_pos][2 * static_cast<int>(figures[player][index]._type) +
                                                                  static_cast<int>(figures[player][index]._color)];
            figures[player][index]._coords = {move.to.x + rook_delta, move.to.y};
            change_eval(player, +1, index, {move.to.x + rook_delta, move.to.y});
            hash ^= zobrist_table[8 * move.to.y + move.to.x + rook_delta][
                    2 * static_cast<int>(figures[player][index]._type) +
                    static_cast<int>(figures[player][index]._color)];
        }
    }
    change_eval(player, +1, i, move.to);
    hash ^= zobrist_table[8 * move.to.y + move.to.x][2 * static_cast<int>(figures[player][i]._type) +
                                                     static_cast<int>(figures[player][i]._color)];

    history[this->hash]++;

    return index_in_figures;
}

static bool if_not_pawn(const Figure &figure) {
    return figure._type != Type::PAWN;
}

static bool is_need_to_mop_up(const std::vector<std::vector<Figure>> &figures, int gamephase) {
    return figures[0].size() + figures[1].size() <= 5 &&
           std::ranges::all_of(figures[0].begin(), figures[0].end(), if_not_pawn) &&
           std::ranges::all_of(figures[1].begin(), figures[1].end(), if_not_pawn) &&
           gamephase > 12;
}

int ArtificialIntelligence::evaluate(int player) const {
    counter++;

    if (is_need_to_mop_up(figures, gamephase)) {
        return mop_up_eval(endgame_piece_eval[0] > endgame_piece_eval[1] ? 1 : 0);
    } else {
        int middlegame_phase = std::min(gamephase, 24);
        return ((middlegame_piece_eval[player] - middlegame_piece_eval[1 - player]) * middlegame_phase +
                (endgame_piece_eval[player] - endgame_piece_eval[1 - player]) * (24 - middlegame_phase)) / 24;
    }
}

void ArtificialIntelligence::change_eval(int player, int sign, int i, Cell cell) {
    if (player == 0) {
        if (_color == Color::WHITE) {
            middlegame_piece_eval[player] +=
                    (middlegame_tables[static_cast<int>(figures[player][i]._type)][cell.y][cell.x] +
                     middlegame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
            endgame_piece_eval[player] +=
                    (endgame_tables[static_cast<int>(figures[player][i]._type)][cell.y][cell.x] +
                     endgame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
        } else {
            middlegame_piece_eval[player] +=
                    (middlegame_tables[static_cast<int>(figures[player][i]._type)][7 - cell.y][7 - cell.x] +
                     middlegame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
            endgame_piece_eval[player] +=
                    (endgame_tables[static_cast<int>(figures[player][i]._type)][7 - cell.y][7 - cell.x] +
                     endgame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
        }
    } else {
        if (_color == Color::BLACK) {
            middlegame_piece_eval[player] +=
                    (middlegame_tables[static_cast<int>(figures[player][i]._type)][cell.y][cell.x] +
                     middlegame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
            endgame_piece_eval[player] +=
                    (endgame_tables[static_cast<int>(figures[player][i]._type)][cell.y][cell.x] +
                     endgame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
        } else {
            middlegame_piece_eval[player] +=
                    (middlegame_tables[static_cast<int>(figures[player][i]._type)][7 - cell.y][7 - cell.x] +
                     middlegame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
            endgame_piece_eval[player] +=
                    (endgame_tables[static_cast<int>(figures[player][i]._type)][7 - cell.y][7 - cell.x] +
                     endgame_piece_value[static_cast<int>(figures[player][i]._type)]) * sign;
        }
    }
}

void ArtificialIntelligence::hash_position(const Board &board) const {
    hash = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]._type == Type::EMPTY)
                continue;
            hash ^= zobrist_table[8 * i + j][2 * static_cast<int>(board[i][j]._type) +
                                             static_cast<int>(board[i][j]._color)];
        }
    }
}

ArtificialIntelligence::ArtificialIntelligence(Color color, Cell king_position) : Intelligence(color, king_position) {
    gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    std::ifstream input("../database/opening_book.txt");

    for (auto &row : zobrist_table) {
        for (auto &value : row) {
            input >> value;
        }
    }

    unsigned long long cur_position = 0;
    Move cur_move;
    int cur_freq;
    std::string raw_string;
    while (std::getline(input, raw_string)) {
        std::stringstream opening_table_row(raw_string);
        opening_table_row >> cur_position;
        while (opening_table_row >> raw_string) {
            cur_move.from.x = raw_string[0] - '0';
            cur_move.from.y = raw_string[1] - '0';
            cur_move.to.x = raw_string[2] - '0';
            cur_move.to.y = raw_string[3] - '0';
            opening_table_row >> cur_freq;
            opening_table[cur_position].first.emplace_back(cur_move);
            opening_table[cur_position].second.emplace_back(cur_freq);
        }
    }

    input.close();
}

int ArtificialIntelligence::positions_count(const Board &board) const {
    hash_position(board);
    return history.at(hash);
}

void ArtificialIntelligence::store_position(const Board &board) {
    hash_position(board);
    history[hash]++;
}

static int get_manhattan_to_center(Cell cell) {
    return std::max(cell.x - 4, 3 - cell.x) + std::max(cell.y - 4, 3 - cell.y);
}

int ArtificialIntelligence::mop_up_eval(int losing_player) const {
    Cell losing_king = losing_player == 0 ? _king_position : _opponent_king_position;
    Cell winning_king = losing_player == 0 ? _opponent_king_position : _king_position;
    return 10 * get_manhattan_to_center(losing_king) +
           4 * (14 - (std::abs(winning_king.x - losing_king.x) + std::abs(winning_king.y - losing_king.y)));
}

