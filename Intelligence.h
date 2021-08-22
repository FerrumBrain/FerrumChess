#pragma once

#include "PawnController.h"
#include "KnightController.h"
#include "BishopController.h"
#include "RookController.h"
#include "QueenController.h"
#include "KingController.h"
#include <map>
#include <memory>

class Intelligence {
public:
    virtual void make_move(Board &board, History &history) = 0;
    map<Type, shared_ptr<FigureController>> controller_container;
    pair<int, int> _king_position;
    Color _color;
    Intelligence(Color color, pair<int, int> king_position){
        controller_container[Type::PAWN] = make_shared<PawnController>();
        controller_container[Type::KNIGHT] = make_shared<KnightController>();
        controller_container[Type::BISHOP] = make_shared<BishopController>();
        controller_container[Type::ROOK] = make_shared<RookController>();
        controller_container[Type::QUEEN] = make_shared<QueenController>();
        controller_container[Type::KING] = make_shared<KingController>();
        _color = color;
        _king_position = king_position;
    };
};
