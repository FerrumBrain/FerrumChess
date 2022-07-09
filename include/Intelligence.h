#pragma once

#include "controllers/PawnController.h"
#include "controllers/KnightController.h"
#include "controllers/BishopController.h"
#include "controllers/RookController.h"
#include "controllers/QueenController.h"
#include "controllers/KingController.h"
#include <map>
#include <memory>

class Intelligence {
public:
    virtual Move make_move(Board &board, Move last_move, int &last_move_for_50move) = 0;

    std::map<Type, std::shared_ptr<FigureController>> controller_container;
    Cell _king_position;
    Color _color;

    Intelligence(Color color, Cell king_position) : _king_position(king_position), _color(color) {
        controller_container[Type::PAWN] = std::make_shared<PawnController>();
        controller_container[Type::KNIGHT] = std::make_shared<KnightController>();
        controller_container[Type::BISHOP] = std::make_shared<BishopController>();
        controller_container[Type::ROOK] = std::make_shared<RookController>();
        controller_container[Type::QUEEN] = std::make_shared<QueenController>();
        controller_container[Type::KING] = std::make_shared<KingController>();
    };

    virtual ~Intelligence() = default;
};
