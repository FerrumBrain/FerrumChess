#include "../include/ArtificialIntelligence.h"

#include <random>

std::vector<std::vector<Figure>> get_figures_coords(const Board &board, Color color) {
    std::vector<std::vector<Figure>> coords(2);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]._color == color) {
                coords[0].emplace_back(board[i][j]);
            } else if (board[i][j]._color == opposite(color)) {
                coords[1].emplace_back(board[i][j]);
            }
        }
    }

    return coords;
}

static bool is_better(double eval1, double eval2) {
    return eval1 > eval2;
}

Move ArtificialIntelligence::make_move(Board &board, Move last_move) {
    Board board_copy = board;
    counter = 0;
    figures = get_figures_coords(board, _color);
    for (const auto &figure : figures[1]) {
        if (figure._type == Type::KING) {
            _opponent_king_position = figure._coords;
            break;
        }
    }

    std::vector<Cell> current_moves;
    auto[ans, promote_to] = search(board_copy, last_move, DEPTH, -BIG_INFINITY, BIG_INFINITY).second;

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

double ArtificialIntelligence::evaluate(Board &board, Move last_move, Color move) const {
    counter++;
    Features features = get_features(0, board, last_move);
    Features opponent_features = get_features(1, board, last_move);

    double eval;
    if (move == _color && features.mobility == 0) {
        if (KingController::is_attacked(_king_position, opposite(_color), board)) {
            eval = (_color == Color::WHITE ? -1 : 1) * INF;
        } else {
            eval = 0;
        }
    } else if (move != _color && opponent_features.mobility == 0) {
        if (KingController::is_attacked(_opponent_king_position, _color, board)) {
            eval = (_color == Color::WHITE ? 1 : -1) * INF;
        } else {
            eval = 0;
        }
    } else if (is_insufficient(figures[0]) && is_insufficient(figures[1])) {
        eval = 0;
    } else {
        eval = (200 * (features.kings - opponent_features.kings) +
                9 * (features.queens - opponent_features.queens) +
                5 * (features.rooks - opponent_features.rooks) +
                3 * (features.bishops - opponent_features.bishops) +
                3 * (features.knights - opponent_features.knights) +
                1 * (features.pawns - opponent_features.pawns) +
                0.1 * (features.mobility - opponent_features.mobility)) * (_color == Color::WHITE ? 1 : -1);
    }
    return eval * (move == Color::WHITE ? 1 : -1); // other features?
}

void ArtificialIntelligence::undo_move(Board &board, Move last_move, const Figure &old_figure_from,
                                       const Figure &old_figure_to, int player, int i, int index_in_figures) {
    board[last_move.from.y][last_move.from.x] = old_figure_from;
    board[last_move.to.y][last_move.to.x] = old_figure_to;
    figures[player][i] = old_figure_from;
    if (index_in_figures != -1) {
        figures[1 - player].insert(figures[1 - player].begin() + index_in_figures, old_figure_to);
    }
    if (figures[player][i]._type == Type::KING) {
        set_king_position(player, last_move.from);
        if (abs(last_move.from.x - last_move.to.x) == 2) {
            int rook_delta = last_move.from.x - last_move.to.x < 0 ? -1 : 1;
            int rook_start_pos = last_move.from.x - last_move.to.x < 0 ? 7 : 0;
            board[last_move.from.y][rook_start_pos] = board[last_move.to.y][last_move.to.x + rook_delta];
            board[last_move.from.y][rook_start_pos]._coords = {rook_start_pos, last_move.to.y};
            figures[player][std::ranges::find_if(figures[player].begin(), figures[player].end(),
                                                 [&last_move, &rook_delta](const Figure &figure) {
                                                     return figure._coords ==
                                                            Cell{last_move.to.x + rook_delta, last_move.to.y};
                                                 }) - figures[player].begin()]._coords = {rook_start_pos,
                                                                                          last_move.to.y};
            board[last_move.to.y][last_move.to.x + rook_delta] = NONE;
        }
    }
}

//Well, SonarLint asks to mark this method as const, but it's static, so...
//NOLINTNEXTLINE
ArtificialIntelligence::Features
ArtificialIntelligence::get_features(int player, Board &board, Move last_move) const {
    Features res;

    for (const auto &figure: figures[player]) {
        switch (figure._type) {
            case Type::PAWN:
                res.pawns++;
                break;
            case Type::ROOK:
                res.rooks++;
                break;
            case Type::KNIGHT:
                res.knights++;
                break;
            case Type::BISHOP:
                res.bishops++;
                break;
            case Type::QUEEN:
                res.queens++;
                break;
            case Type::KING:
                res.kings++;
                break;
            default:
                break;
        }
        res.mobility += static_cast<int>(controller_container.at(
                board[figure._coords.y][figure._coords.x]._type)->get_moves(figure._coords,
                                                                            board,
                                                                            last_move,
                                                                            player == 0 ? _king_position
                                                                                        : _opponent_king_position).size());
    }

    return res;
}

void ArtificialIntelligence::set_king_position(int player, Cell new_king_position) {
    if (player == 0) _king_position = new_king_position;
    else _opponent_king_position = new_king_position;
}

std::pair<double, std::pair<Move, Type>>
ArtificialIntelligence::search(Board &board, Move last_move, int depth, double alpha, double beta) {
    int player = (DEPTH - depth) % 2;
    if (depth == 0)
        return {evaluate(board, last_move, (player == 1 ? opposite(_color) : _color)), {Move(), Type::EMPTY}};

    std::vector<std::pair<int, std::pair<Move, Type>>> possible_moves = get_possible_moves(player, last_move, board);

    if (possible_moves.empty()) {
        if (KingController::is_attacked(player == 1 ? _opponent_king_position : _king_position,
                                        player == 1 ? _color : opposite(_color), board)) {
            return {(depth + 1) * -INF, {Move(), Type::EMPTY}};
        }
        return {0, {Move(), Type::EMPTY}};
    }

    Move ans;
    Type promote_to = Type::EMPTY;

    for (auto const &[i, move_and_promote_to] : possible_moves) {
        auto move = move_and_promote_to.first;
        auto possible_promote_to = move_and_promote_to.second;
        Cell old_coords = figures[player][i]._coords;
        auto old_figure_from = figures[player][i];
        Figure old_figure_to = board[move.to.y][move.to.x];
        // -------------------------------------------------------------------------------------------------------------
        int index_in_figures = make_pseudo_move(player, i, move, old_figure_to, board, last_move, possible_promote_to);
        // -------------------------------------------------------------------------------------------------------------
        double cur_eval = -search(board, {old_coords, move.to}, depth - 1, -beta, -alpha).first;
        // -------------------------------------------------------------------------------------------------------------
        undo_move(board, {old_coords, move.to}, old_figure_from, old_figure_to, player, i, index_in_figures);
        // -------------------------------------------------------------------------------------------------------------
        if (cur_eval >= beta) {
            return {beta, {Move(), Type::EMPTY}};
        }

        if (is_better(cur_eval, alpha)) {
            alpha = cur_eval;
            ans = {old_coords, move.to};
            promote_to = possible_promote_to;
        }
    }

    return {alpha, {ans, promote_to}};
}

void ArtificialIntelligence::reorder_moves(int player, Move last_move, Board &board,
                                           std::vector<std::pair<int, std::pair<Move, Type>>> &possible_moves) {
    auto is_attack = [this, &board, &player, &last_move](Move move, Type promote_to, int i) {
        Cell old_coords = move.from;
        auto old_figure_from = board[move.from.y][move.from.x];
        Figure old_figure_to = board[move.to.y][move.to.x];
        int index_in_figures = make_pseudo_move(player, i, move, old_figure_to, board, last_move, promote_to);

        auto possible_cells = controller_container[old_figure_from._type]->get_moves(move.to, board, move,
                                                                                     player == 0 ? _king_position
                                                                                                 : _opponent_king_position);
        bool res = std::ranges::any_of(possible_cells.begin(), possible_cells.end(), [&board](Cell a) {
            return board[a.y][a.x] != NONE;
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

    std::ranges::sort(possible_moves.begin(), possible_moves.end(),
                      [&](const std::pair<int, std::pair<Move, Type>> &a,
                          const std::pair<int, std::pair<Move, Type>> &b) {
                          Move move_a = a.second.first;
                          Type promote_to_a = a.second.second;
                          Move move_b = b.second.first;
                          Type promote_to_b = b.second.second;
                          int priority_a = 0;
                          int priority_b = 0;

                          if (is_check(move_a, promote_to_a, a.first)) priority_a = 3;
                          if (is_check(move_b, promote_to_b, b.first)) priority_b = 3;

                          if (priority_a == 0 && is_capture(move_a, promote_to_a, a.first)) priority_a = 2;
                          if (priority_b == 0 && is_capture(move_b, promote_to_b, b.first)) priority_b = 2;

                          if (priority_a == 0 && is_attack(move_a, promote_to_a, a.first)) priority_a = 1;
                          if (priority_b == 0 && is_attack(move_b, promote_to_b, b.first)) priority_b = 1;

                          return priority_a > priority_b; // We need to look at high-priority moves first
                      });
}

std::vector<std::pair<int, std::pair<Move, Type>>> ArtificialIntelligence::get_possible_moves(int player,
                                                                                              Move last_move,
                                                                                              Board &board) {
    std::vector<std::pair<int, std::pair<Move, Type>>> possible_moves;
    std::vector<Cell> current_cells;
    for (int i = 0; i < figures[player].size(); i++) {
        current_cells = controller_container[figures[player][i]._type]->get_moves(figures[player][i]._coords, board,
                                                                                  last_move,
                                                                                  player == 0 ? _king_position
                                                                                              : _opponent_king_position);
        for (auto to : current_cells) {
            if (figures[player][i]._type == Type::PAWN && to.y % 7 == 0) {
                possible_moves.push_back({i, {{figures[player][i]._coords, to}, Type::QUEEN}});
                possible_moves.push_back({i, {{figures[player][i]._coords, to}, Type::ROOK}});
                possible_moves.push_back({i, {{figures[player][i]._coords, to}, Type::BISHOP}});
                possible_moves.push_back({i, {{figures[player][i]._coords, to}, Type::KNIGHT}});
            } else {
                possible_moves.push_back({i, {{figures[player][i]._coords, to}, Type::EMPTY}});
            }
        }
    }

    reorder_moves(player, last_move, board, possible_moves);
    // std::ranges::shuffle(possible_moves.begin(), possible_moves.end(), std::mt19937(std::random_device()()));

    return possible_moves;
}

int ArtificialIntelligence::make_pseudo_move(int player, int i, Move move, const Figure &old_figure_to, Board &board,
                                             Move last_move, Type possible_promote_to) {
    int index_in_figures = -1;
    if (old_figure_to != NONE) {
        auto iter = std::ranges::find_if(figures[1 - player].begin(), figures[1 - player].end(),
                                         [&](const Figure &figure) {
                                             return figure._coords == move.to;
                                         });
        index_in_figures = static_cast<int>(iter - figures[1 - player].begin());
        figures[1 - player].erase(iter);
    }
    controller_container[figures[player][i]._type]->make_move(
            {figures[player][i]._coords, move.to}, board, last_move, possible_promote_to);
    figures[player][i] = board[move.to.y][move.to.x];
    if (figures[player][i]._type == Type::KING) {
        set_king_position(player, move.to);
        if (abs(move.from.x - move.to.x) == 2) {
            int rook_delta = move.from.x - move.to.x < 0 ? -1 : 1;
            int rook_start_pos = move.from.x - move.to.x < 0 ? 7 : 0;
            figures[player][std::ranges::find_if(figures[player].begin(), figures[player].end(),
                                                 [&move, &rook_start_pos](const Figure &figure) {
                                                     return figure._coords == Cell{rook_start_pos, move.to.y};
                                                 }) - figures[player].begin()]._coords = {move.to.x + rook_delta,
                                                                                          move.to.y};
        }
    }

    return index_in_figures;
}
