#include "../include/ArtificialIntelligence.h"

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

static bool is_better(double eval1, double eval2) {
    return eval1 < eval2;
}

void
ArtificialIntelligence::try_move(int i, std::pair<Cell, Move> to_and_last_move, Board &board, double &best_eval,
                                 Move &ans, Type &promote_to_ref, Type promote_to) {

    auto const&[to, last_move] = to_and_last_move;
    Cell old_coords = figures[i]._coords;
    Figure old_figure_to = board[to.y][to.x];
    controller_container[figures[i]._type]->make_move({figures[i]._coords, to}, board, last_move, promote_to);
    if (figures[i]._type == Type::KING) {
        _king_position = to;
    }
    figures[i]._coords = to;
    if (double cur_eval = evaluate(board, {old_coords, to}, opposite(_color));
            best_eval == std::numeric_limits<double>::infinity() ||
            is_better(cur_eval, best_eval)) {
        best_eval = cur_eval;
        ans = {old_coords, to};
        promote_to_ref = promote_to;
    }
    undo_move(board, {old_coords, to}, figures[i], old_figure_to);
    figures[i]._coords = old_coords;
    if (figures[i]._type == Type::KING) {
        _king_position = old_coords;
    }
}

Move ArtificialIntelligence::make_move(Board &board, Move last_move) {
    figures = get_figures_coords(board, _color);

    std::vector<Cell> current_moves;

    Move ans;
    Type promote_to = Type::EMPTY;
    double best_eval = std::numeric_limits<double>::infinity();

    for (int i = 0; i < figures.size(); i++) {
        current_moves = controller_container[figures[i]._type]->get_moves(figures[i]._coords, board, last_move,
                                                                          _king_position);
        for (auto to : current_moves) {
            if (figures[i]._type == Type::PAWN && to.y == (_color == Color::WHITE ? 7 : 0)) {
                try_move(i, {to, last_move}, board, best_eval, ans, promote_to, Type::QUEEN);
                try_move(i, {to, last_move}, board, best_eval, ans, promote_to, Type::ROOK);
                try_move(i, {to, last_move}, board, best_eval, ans, promote_to, Type::KNIGHT);
                try_move(i, {to, last_move}, board, best_eval, ans, promote_to, Type::BISHOP);
            } else {
                try_move(i, {to, last_move}, board, best_eval, ans, promote_to);
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
    Cell opponent_king_position;
    for (const auto &figure: opponent_figures) {
        if (figure._type == Type::KING) opponent_king_position = figure._coords;
    }

    Features features = get_features(figures, board, last_move, _king_position);
    Features opponent_features = get_features(opponent_figures, board, last_move, opponent_king_position);

    double eval;
    if (move == _color && features.mobility == 0) {
        if (KingController::is_attacked(_king_position, opposite(_color), board)) {
            eval = (_color == Color::WHITE ? -1 : 1) * 1e9;
        } else {
            eval = 0;
        }
    } else if (move != _color && opponent_features.mobility == 0) {
        if (KingController::is_attacked(opponent_king_position, _color, board)) {
            eval = (_color == Color::WHITE ? 1 : -1) * 1e9;
        } else {
            eval = 0;
        }
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
                                       const Figure &old_figure_to) {
    board[last_move.from.y][last_move.from.x] = old_figure_from;
    board[last_move.to.y][last_move.to.x] = old_figure_to;
}

//Well, SonarLint asks to mark this method as const, but it's static, so...
//NOLINTNEXTLINE
ArtificialIntelligence::Features
ArtificialIntelligence::get_features(const std::vector<Figure> &cur_figures, const Board &board, Move last_move,
                                     Cell king_position) {
    Features res;

    for (const auto &figure: cur_figures) {
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
                                                                            king_position).size());
    }

    return res;
}
