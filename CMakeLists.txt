cmake_minimum_required(VERSION 3.17)
project(FerrumChess)

set(CMAKE_CXX_STANDARD 20)

add_executable(FerrumChess main.cpp Type.h Figure.h Color.h FigureController.h KnightController.cpp KnightController.h BishopController.cpp BishopController.h RookController.cpp RookController.h QueenController.cpp QueenController.h constants.h KingController.cpp KingController.h PawnController.cpp PawnController.h Intelligence.h UserIntelligence.cpp UserIntelligence.h ArtificialIntelligence.cpp ArtificialIntelligence.h)