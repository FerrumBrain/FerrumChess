cmake_minimum_required(VERSION 3.17)
project(FerrumChess)

set(CMAKE_CXX_STANDARD 20)

set(CLIENT_PROJECT_SOURCES
        src/ArtificialIntelligence.cpp
        src/controllers/BishopController.cpp
        src/controllers/FigureController.cpp
        src/GameController.cpp
        src/controllers/KingController.cpp
        src/controllers/KnightController.cpp
        src/main.cpp
        src/controllers/PawnController.cpp
        src/controllers/QueenController.cpp
        src/controllers/RookController.cpp
        src/UserIntelligence.cpp
        src/UIController.cpp
        )
add_executable(FerrumChess ${CLIENT_PROJECT_SOURCES})
if (DEBUG_LEVEL)
    add_compile_definitions(FerrumChess DEBUG)
endif ()
add_executable(OpeningBook database/aggregate_openings.cpp)
target_link_libraries(FerrumChess ${CLIENT_LIBS})