cmake_minimum_required(VERSION 3.17)
project(FerrumChess)

set(CMAKE_CXX_STANDARD 20)

find_package(TGUI 0.10 COMPONENTS tgui)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)
add_definitions(-DSFML_STATIC)

set(CLIENT_PROJECT_SOURCES
        src/ArtificialIntelligence.cpp
        src/BishopController.cpp
        src/FigureController.cpp
        src/GameController.cpp
        src/KingController.cpp
        src/KnightController.cpp
        src/main.cpp
        src/PawnController.cpp
        src/QueenController.cpp
        src/RookController.cpp
        src/UserIntelligence.cpp
        )
set(CLIENT_LIBS sfml-system sfml-window sfml-graphics sfml-audio tgui)
add_executable(FerrumChess ${CLIENT_PROJECT_SOURCES})
target_link_libraries(FerrumChess ${CLIENT_LIBS})