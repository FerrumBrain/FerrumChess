#include "../include/ArtificialIntelligence.h"
#include "../include/GameController.h"

std::vector<Figure> get_figures_coords(const Board &board, Color color) {
    std::vector<Figure> coords;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j]._color == color) {
                coords.emplace_back(board[i][j]);
            }
        }
    }

    return coords;
}

Move ArtificialIntelligence::make_move(Board &board, Move last_move) {
    figures = get_figures_coords(board, _color);

    std::vector<Cell> current_moves;

    Move ans;
    Type promote_to = Type::EMPTY;
    double best_eval = std::numeric_limits<double>::infinity();

    for (auto &figure: figures) {
        current_moves = controller_container[figure._type]->get_moves(figure._coords, board, last_move,
                                                                      _king_position);
        for (auto to : current_moves) {
            // Make move
            // Evaluate
            // Unmake move
            double cur_eval;
            Cell old_coords = figure._coords;
            Figure old_figure_to = board[to.y][to.x];
            if (figure._type == Type::PAWN && to.y == (_color == Color::WHITE ? 7 : 0)) {
                controller_container[figure._type]->make_move({figure._coords, to}, board, last_move, Type::QUEEN);
                figure._coords = to;
                cur_eval = evaluate(board, {old_coords, to}, opposite(_color));
                if (best_eval == std::numeric_limits<double>::infinity() ||
                    is_better(cur_eval, best_eval, opposite(_color))) {
                    best_eval = cur_eval;
                    ans = {old_coords, to};
                    promote_to = Type::QUEEN;
                }
                undo_move(board, {old_coords, to}, figure, old_figure_to);
                figure._coords = old_coords;


                controller_container[figure._type]->make_move({figure._coords, to}, board, last_move, Type::ROOK);
                figure._coords = to;
                cur_eval = evaluate(board, {old_coords, to}, opposite(_color));
                if (best_eval == std::numeric_limits<double>::infinity() ||
                    is_better(cur_eval, best_eval, opposite(_color))) {
                    best_eval = cur_eval;
                    ans = {old_coords, to};
                    promote_to = Type::ROOK;
                }
                undo_move(board, {old_coords, to}, figure, old_figure_to);
                figure._coords = old_coords;

                controller_container[figure._type]->make_move({figure._coords, to}, board, last_move, Type::BISHOP);
                figure._coords = to;
                cur_eval = evaluate(board, {old_coords, to}, opposite(_color));
                if (best_eval == std::numeric_limits<double>::infinity() ||
                    is_better(cur_eval, best_eval, opposite(_color))) {
                    best_eval = cur_eval;
                    ans = {old_coords, to};
                    promote_to = Type::BISHOP;
                }
                undo_move(board, {old_coords, to}, figure, old_figure_to);
                figure._coords = old_coords;

                controller_container[figure._type]->make_move({figure._coords, to}, board, last_move, Type::KNIGHT);
                figure._coords = to;
                cur_eval = evaluate(board, {old_coords, to}, opposite(_color));
                if (best_eval == std::numeric_limits<double>::infinity() ||
                    is_better(cur_eval, best_eval, opposite(_color))) {
                    best_eval = cur_eval;
                    ans = {old_coords, to};
                    promote_to = Type::KNIGHT;
                }
                undo_move(board, {old_coords, to}, figure, old_figure_to);
                figure._coords = old_coords;
            } else {
                controller_container[figure._type]->make_move({figure._coords, to}, board, last_move, Type::EMPTY);
                figure._coords = to;
                if(figure._type == Type::KING) {
                    _king_position = to;
                }
                cur_eval = evaluate(board, {old_coords, to}, opposite(_color));
                if (best_eval == std::numeric_limits<double>::infinity() ||
                    is_better(cur_eval, best_eval, opposite(_color))) {
                    best_eval = cur_eval;
                    ans = {old_coords, to};
                }
                undo_move(board, {old_coords, to}, figure, old_figure_to);
                figure._coords = old_coords;
                if(figure._type == Type::KING) {
                    _king_position = old_coords;
                }
            }
        }
    }

    auto[from, to] = ans;
    Type type = board[from.y][from.x]._type;
    controller_container[type]->make_move(ans, board, last_move, promote_to);
    if (board[to.y][to.x]._type == Type::KING)
        _king_position = to;

    return ans;
}

double ArtificialIntelligence::evaluate(const Board &board, Move last_move, Color move) const {
    auto opponent_figures = get_figures_coords(board, (_color == Color::WHITE) ? Color::BLACK : Color::WHITE);

    int pawns = 0;
    int rooks = 0;
    int knights = 0;
    int bishops = 0;
    int queens = 0;
    int kings = 0;
    int opponent_pawns = 0;
    int opponent_rooks = 0;
    int opponent_knights = 0;
    int opponent_bishops = 0;
    int opponent_queens = 0;
    int mobility = 0;
    int opponent_mobility = 0;
    int opponent_kings = 0;
    Cell opponent_king_position;

    for (const auto &figure: figures) {
        switch (figure._type) {
            case Type::PAWN:
                pawns++;
                break;
            case Type::ROOK:
                rooks++;
                break;
            case Type::KNIGHT:
                knights++;
                break;
            case Type::BISHOP:
                bishops++;
                break;
            case Type::QUEEN:
                queens++;
                break;
            case Type::KING:
                kings++;
                break;
            default:
                break;
        }
        mobility += static_cast<int>(controller_container.at(
                board[figure._coords.y][figure._coords.x]._type)->get_moves(figure._coords,
                                                                            board,
                                                                            last_move,
                                                                            _king_position).size());
    }

    for (const auto &figure: opponent_figures) {
        if (figure._type == Type::KING) opponent_king_position = figure._coords;
    }

    for (const auto &figure: opponent_figures) {
        switch (figure._type) {
            case Type::PAWN:
                opponent_pawns++;
                break;
            case Type::ROOK:
                opponent_rooks++;
                break;
            case Type::KNIGHT:
                opponent_knights++;
                break;
            case Type::BISHOP:
                opponent_bishops++;
                break;
            case Type::QUEEN:
                opponent_queens++;
                break;
            case Type::KING:
                opponent_kings++;
                break;
            default:
                break;
        }
        opponent_mobility += static_cast<int>(controller_container.at(
                board[figure._coords.y][figure._coords.x]._type)->get_moves(figure._coords,
                                                                            board,
                                                                            last_move,
                                                                            opponent_king_position).size());
    }

    double eval;
    if (move == _color && mobility == 0) {
        if (KingController::is_attacked(_king_position, opposite(_color), board)) {
            eval = (_color == Color::WHITE ? -1 : 1) * 1e9;
        } else {
            eval = 0;
        }
    } else if (move != _color && opponent_mobility == 0) {
        if (KingController::is_attacked(opponent_king_position, _color, board)) {
            eval = (_color == Color::WHITE ? 1 : -1) * 1e9;
        } else {
            eval = 0;
        }
    } else {
        eval = (200 * (kings - opponent_kings) +
                9 * (queens - opponent_queens) +
                5 * (rooks - opponent_rooks) +
                3 * (bishops - opponent_bishops) +
                3 * (knights - opponent_knights) +
                1 * (pawns - opponent_pawns) +
                0.1 * (mobility - opponent_mobility)) * (_color == Color::WHITE ? 1 : -1);
    }
    return eval * (move == Color::WHITE ? 1 : -1); // other features?
}

bool ArtificialIntelligence::is_better(double eval1, double eval2, Color color) {
    int k = -1;
    return k * eval1 > k * eval2;
}

void ArtificialIntelligence::undo_move(Board &board, Move last_move, const Figure &old_figure_from,
                                       const Figure &old_figure_to) {
    board[last_move.from.y][last_move.from.x] = old_figure_from;
    board[last_move.to.y][last_move.to.x] = old_figure_to;
}
