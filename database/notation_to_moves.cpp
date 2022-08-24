#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

const int NUM_OF_OPENING_MOVES = 15;
const int INF = 1000;

std::vector<std::vector<std::pair<int, int>>> pawns;
std::vector<std::vector<std::pair<int, int>>> rooks;
std::vector<std::vector<std::pair<int, int>>> knights;
std::vector<std::vector<std::pair<int, int>>> bishops;
std::vector<std::vector<std::pair<int, int>>> queens;
std::vector<std::pair<int, int>> kings;
std::vector<std::vector<int>> board;

const int WHITE = 0;
const int BLACK = 1;

const int PAWN = 0;
const int KNIGHT = 1;
const int BISHOP = 2;
const int ROOK = 3;
const int QUEEN = 4;
const int KING = 5;

int sign(int x) {
    return x < 0 ? -1 : (x == 0 ? 0 : 1);
}

std::string pair_to_string(std::pair<int, int> coords) {
    return std::to_string(coords.first) + std::to_string(coords.second);
}

void handle_taking(std::pair<int, int> coords, int player) {
    if (board[coords.second][coords.first] == -1) return;
    switch (board[coords.second][coords.first] / 2) {
        case 0:
            pawns[player][std::ranges::find(pawns[player].begin(), pawns[player].end(), coords) -
                          pawns[player].begin()] = {-INF, -INF + 50};
            break;
        case 1:
            knights[player][std::ranges::find(knights[player].begin(), knights[player].end(), coords) -
                            knights[player].begin()] = {-INF, -INF + 50};
            break;
        case 2:
            bishops[player][std::ranges::find(bishops[player].begin(), bishops[player].end(), coords) -
                            bishops[player].begin()] = {-INF, -INF + 50};
            break;
        case 3:
            rooks[player][std::ranges::find(rooks[player].begin(), rooks[player].end(), coords) -
                          rooks[player].begin()] = {-INF, -INF + 50};
            break;
        case 4:
            queens[player][std::ranges::find(queens[player].begin(), queens[player].end(), coords) -
                           queens[player].begin()] = {-INF, -INF + 50};
            break;
        default:
            assert(false);
    }
}

std::string handle_castle(int move_counter, const std::string &move_notation) {
    std::string res;
    if (move_notation.size() == 3) { // Short castle
        res = pair_to_string(kings[move_counter % 2]);
        int old = board[kings[move_counter % 2].second][kings[move_counter % 2].first];
        board[kings[move_counter % 2].second][kings[move_counter % 2].first] = -1;
        kings[move_counter % 2].first += 2;
        board[kings[move_counter % 2].second][kings[move_counter % 2].first] = old;
        res += pair_to_string(kings[move_counter % 2]);
        assert(rooks[move_counter % 2][1].first == 7);
        old = board[rooks[move_counter % 2][1].second][rooks[move_counter % 2][1].first];
        board[rooks[move_counter % 2][1].second][rooks[move_counter % 2][1].first] = -1;
        rooks[move_counter % 2][1].first -= 2;
        board[rooks[move_counter % 2][1].second][rooks[move_counter % 2][1].first] = old;
    } else { // Long castle
        res = pair_to_string(kings[move_counter % 2]);
        int old = board[kings[move_counter % 2].second][kings[move_counter % 2].first];
        board[kings[move_counter % 2].second][kings[move_counter % 2].first] = -1;
        kings[move_counter % 2].first -= 2;
        board[kings[move_counter % 2].second][kings[move_counter % 2].first] = old;
        res += pair_to_string(kings[move_counter % 2]);
        assert(rooks[move_counter % 2][0].first == 0);
        old = board[rooks[move_counter % 2][0].second][rooks[move_counter % 2][0].first];
        board[rooks[move_counter % 2][0].second][rooks[move_counter % 2][0].first] = -1;
        rooks[move_counter % 2][0].first += 3;
        board[rooks[move_counter % 2][0].second][rooks[move_counter % 2][0].first] = old;
    }
    return res;
}

std::string handle_pawn(int move_counter, const std::string &move_notation) {
    // In first 15 moves there aren't any promotions, so we can forget about them
    std::string res;

    if (std::isupper(move_notation.back())) {
        assert(false);
    }

    int sign = -(2 * (move_counter % 2) - 1);
    auto iter_for_1_square_move = std::ranges::find_if(pawns[move_counter % 2].begin(),
                                                       pawns[move_counter % 2].end(),
                                                       [&move_notation, sign](
                                                               const std::pair<int, int> &coords) {
                                                           return coords.first ==
                                                                  (move_notation[0] - 'a') &&
                                                                  coords.second ==
                                                                  (move_notation.back() - '1' - sign);
                                                       }) - pawns[move_counter % 2].begin();
    if (move_notation.size() == 3) { // Pawn took
        res = pair_to_string(pawns[move_counter % 2][iter_for_1_square_move]);
        int old = board[pawns[move_counter % 2][iter_for_1_square_move].second][pawns[move_counter %
                                                                                      2][iter_for_1_square_move].first];
        board[pawns[move_counter % 2][iter_for_1_square_move].second][pawns[move_counter %
                                                                            2][iter_for_1_square_move].first] = -1;
        pawns[move_counter % 2][iter_for_1_square_move] = {
                move_notation[1] - 'a', move_notation[2] - '1'};
        if (board[pawns[move_counter % 2][iter_for_1_square_move].second][pawns[move_counter %
                                                                                2][iter_for_1_square_move].first] ==
                    -1) {
            handle_taking({pawns[move_counter % 2][iter_for_1_square_move].first,
                           pawns[move_counter % 2][iter_for_1_square_move].second + (2 * (1 - move_counter % 2) - 1)},
                          1 - move_counter % 2);
            board[pawns[move_counter % 2][iter_for_1_square_move].second - (2 * (1 - move_counter % 2) - 1)][pawns[move_counter % 2][iter_for_1_square_move].first] = -1;
        }
        handle_taking(pawns[move_counter % 2][iter_for_1_square_move], 1 - move_counter % 2);
        board[pawns[move_counter % 2][iter_for_1_square_move].second][pawns[move_counter %
                                                                            2][iter_for_1_square_move].first] = old;
        res += pair_to_string(pawns[move_counter % 2][iter_for_1_square_move]);
    } else {
        if (iter_for_1_square_move != pawns[move_counter % 2].size()) {
            res = pair_to_string(pawns[move_counter % 2][iter_for_1_square_move]);
            int old = board[pawns[move_counter % 2][iter_for_1_square_move].second][pawns[move_counter %
                                                                                          2][iter_for_1_square_move].first];
            board[pawns[move_counter % 2][iter_for_1_square_move].second][pawns[move_counter %
                                                                                2][iter_for_1_square_move].first] = -1;
            pawns[move_counter % 2][iter_for_1_square_move] = {
                    move_notation[0] - 'a', move_notation[1] - '1'};
            board[pawns[move_counter % 2][iter_for_1_square_move].second][pawns[move_counter %
                                                                                2][iter_for_1_square_move].first] = old;
            res += pair_to_string(pawns[move_counter % 2][iter_for_1_square_move]);
        } else {
            auto iter_for_2_square_move = std::ranges::find_if(pawns[move_counter % 2].begin(),
                                                               pawns[move_counter % 2].end(),
                                                               [&move_notation, sign](
                                                                       const std::pair<int, int> &coords) {
                                                                   return coords.first ==
                                                                          (move_notation[0] - 'a') &&
                                                                          coords.second ==
                                                                          (move_notation.back() - '1' -
                                                                           2 * sign);
                                                               }) - pawns[move_counter % 2].begin();
            res = pair_to_string(pawns[move_counter % 2][iter_for_2_square_move]);
            int old = board[pawns[move_counter % 2][iter_for_2_square_move].second][pawns[move_counter %
                                                                                          2][iter_for_2_square_move].first];
            board[pawns[move_counter % 2][iter_for_2_square_move].second][pawns[move_counter %
                                                                                2][iter_for_2_square_move].first] = -1;
            pawns[move_counter % 2][iter_for_2_square_move] = {move_notation[0] - 'a', move_notation[1] - '1'};
            board[pawns[move_counter % 2][iter_for_2_square_move].second][pawns[move_counter %
                                                                                2][iter_for_2_square_move].first] = old;
            res += pair_to_string(pawns[move_counter % 2][iter_for_2_square_move]);
        }
    }
    return res;
}

std::string handle_knight(int move_counter, const std::string &move_notation) {
    auto lambda = [&move_notation](const std::pair<int, int> &coords) {
        if (move_notation.size() == 3) {
            return abs((move_notation[1] - 'a') - coords.first) == 2 &&
                   abs((move_notation[2] - '1') - coords.second) == 1 ||
                   abs((move_notation[1] - 'a') - coords.first) == 1 &&
                   abs((move_notation[2] - '1') - coords.second) == 2;
        } else {
            return (abs((move_notation[2] - 'a') - coords.first) == 2 &&
                    abs((move_notation[3] - '1') - coords.second) == 1 ||
                    abs((move_notation[2] - 'a') - coords.first) == 1 &&
                    abs((move_notation[3] - '1') - coords.second) == 2) &&
                   (coords.first == move_notation[1] - 'a' ||
                    coords.second == move_notation[1] - '1');
        }
    };

    std::string res;
    int iter = -1;
    if (std::ranges::count_if(knights[move_counter % 2].begin(), knights[move_counter % 2].end(), lambda) == 2) {
        iter = static_cast<int>(std::ranges::find_if(knights[move_counter % 2].begin(), knights[move_counter % 2].end(),
                                                     [move_counter](const std::pair<int, int> &coords) {
                                                         std::pair<int, int> direction;
                                                         if (kings[move_counter % 2].first == coords.first) {
                                                             direction = {0, -sign(kings[move_counter % 2].second -
                                                                                   coords.second)};
                                                         } else if (kings[move_counter % 2].second == coords.second) {
                                                             direction = {-sign(kings[move_counter % 2].first -
                                                                                coords.first), 0};
                                                         } else if (abs(kings[move_counter % 2].first - coords.first) ==
                                                                    abs(kings[move_counter % 2].second -
                                                                        coords.second)) {
                                                             direction = {-sign(kings[move_counter % 2].first -
                                                                                coords.first),
                                                                          -sign(kings[move_counter % 2].second -
                                                                                coords.second)};
                                                         } else {
                                                             return true;
                                                         }
                                                         auto[x, y] = kings[move_counter % 2];
                                                         bool is_reached = false;
                                                         while (0 <= x && x <= 7 && 0 <= y && y <= 7) {
                                                             if (std::make_pair(x, y) != kings[move_counter % 2] &&
                                                                 std::make_pair(x, y) != coords) {
                                                                 if (!is_reached) {
                                                                     if (board[y][x] != -1) return true;
                                                                 } else {
                                                                     if (board[y][x] != -1) {
                                                                         if (board[y][x] % 2 == move_counter % 2)
                                                                             return true;
                                                                         else {
                                                                             if (direction.first != 0 &&
                                                                                 direction.second != 0) {
                                                                                 return !(board[y][x] / 2 == QUEEN ||
                                                                                          board[y][x] / 2 == BISHOP);
                                                                             } else {
                                                                                 return !(board[y][x] / 2 == QUEEN ||
                                                                                          board[y][x] / 2 == ROOK);
                                                                             }
                                                                         }
                                                                     }
                                                                 }
                                                             }
                                                             is_reached = is_reached || std::make_pair(x, y) == coords;
                                                             x += direction.first;
                                                             y += direction.second;
                                                         }
                                                         return true;
                                                     }) - knights[move_counter % 2].begin());
    } else {
        iter = static_cast<int>(
                std::ranges::find_if(knights[move_counter % 2].begin(), knights[move_counter % 2].end(), lambda) -
                knights[move_counter % 2].begin());
    }
    res = pair_to_string(knights[move_counter % 2][iter]);
    int old = board[knights[move_counter % 2][iter].second][knights[move_counter % 2][iter].first];
    board[knights[move_counter % 2][iter].second][knights[move_counter % 2][iter].first] = -1;
    knights[move_counter % 2][iter] = {
            move_notation[move_notation.size() - 2] - 'a',
            (move_notation[move_notation.size() - 1] - '1')};
    handle_taking(knights[move_counter % 2][iter], 1 - move_counter % 2);
    board[knights[move_counter % 2][iter].second][knights[move_counter % 2][iter].first] = old;
    res += pair_to_string(knights[move_counter % 2][iter]);
    return res;
}

std::string handle_bishop(int move_counter, const std::string &move_notation) {
    std::string res;
    auto iter = std::ranges::find_if(bishops[move_counter % 2].begin(), bishops[move_counter % 2].end(),
                                     [&move_notation](const std::pair<int, int> &coords) {
                                         return abs((move_notation[1] - 'a') - coords.first) ==
                                                abs((move_notation[2] - '1') - coords.second) ==
                                                1; // There are no promotions, thus bishops are always on different colors
                                     }) - bishops[move_counter % 2].begin();
    res = pair_to_string(bishops[move_counter % 2][iter]);
    int old = board[bishops[move_counter % 2][iter].second][bishops[move_counter % 2][iter].first];
    board[bishops[move_counter % 2][iter].second][bishops[move_counter % 2][iter].first] = -1;
    bishops[move_counter % 2][iter] = {move_notation[1] - 'a', (move_notation[2] - '1')};
    handle_taking(bishops[move_counter % 2][iter], 1 - move_counter % 2);
    board[bishops[move_counter % 2][iter].second][bishops[move_counter % 2][iter].first] = old;
    res += pair_to_string(bishops[move_counter % 2][iter]);
    return res;
}

std::string handle_rook(int move_counter, const std::string &move_notation) {

    auto &b = board;
    auto &k = rooks;
    std::string res;
    auto iter = std::ranges::find_if(rooks[move_counter % 2].begin(), rooks[move_counter % 2].end(),
                                     [&move_notation](const std::pair<int, int> &coords) {
                                         if (move_notation.size() == 3) {
                                             if ((move_notation[1] - 'a') - coords.first == 0) {
                                                 for (int i = std::min((move_notation[2] - '1'), coords.second) + 1;
                                                      i <= std::max((move_notation[2] - '1'), coords.second) - 1; ++i) {
                                                     if (board[i][coords.first] != -1)
                                                         return false;
                                                 }
                                             } else if ((move_notation[2] - '1') - coords.second == 0) {
                                                 for (int i = std::min((move_notation[1] - 'a'), coords.first) + 1;
                                                      i <= std::max((move_notation[1] - 'a'), coords.first) - 1; ++i) {
                                                     if (board[coords.second][i] != -1)
                                                         return false;
                                                 }
                                             } else {
                                                 return false;
                                             }
                                             return true;
                                         } else {
                                             return (coords.first == move_notation[1] - 'a' ||
                                                     coords.second == move_notation[1] - '1');
                                         }
                                     }) - rooks[move_counter % 2].begin();
    res = pair_to_string(rooks[move_counter % 2][iter]);
    int old = board[rooks[move_counter % 2][iter].second][rooks[move_counter % 2][iter].first];
    board[rooks[move_counter % 2][iter].second][rooks[move_counter % 2][iter].first] = -1;
    rooks[move_counter % 2][iter] = {
            move_notation[move_notation.size() - 2] - 'a',
            (move_notation[move_notation.size() - 1] - '1')};
    handle_taking(rooks[move_counter % 2][iter], 1 - move_counter % 2);
    board[rooks[move_counter % 2][iter].second][rooks[move_counter % 2][iter].first] = old;
    res += pair_to_string(rooks[move_counter % 2][iter]);
    return res;
}

std::string handle_queen(int move_counter, const std::string &move_notation) {
    std::string res;
    res = pair_to_string(queens[move_counter % 2][0]);
    int old = board[queens[move_counter % 2][0].second][queens[move_counter % 2][0].first];
    board[queens[move_counter % 2][0].second][queens[move_counter % 2][0].first] = -1;
    queens[move_counter % 2][0] = {move_notation[move_notation.size() - 2] - 'a',
                                   (move_notation[move_notation.size() - 1] - '1')};
    handle_taking(queens[move_counter % 2][0], 1 - move_counter % 2);
    board[queens[move_counter % 2][0].second][queens[move_counter % 2][0].first] = old;
    res += pair_to_string(queens[move_counter % 2][0]);
    return res;
}

std::string handle_king(int move_counter, const std::string &move_notation) {
    std::string res;
    res = pair_to_string(kings[move_counter % 2]);
    int old = board[kings[move_counter % 2].second][kings[move_counter % 2].first];
    board[kings[move_counter % 2].second][kings[move_counter % 2].first] = -1;
    kings[move_counter % 2] = {move_notation[move_notation.size() - 2] - 'a',
                               (move_notation[move_notation.size() - 1] - '1')};
    handle_taking(kings[move_counter % 2], 1 - move_counter % 2);
    board[kings[move_counter % 2].second][kings[move_counter % 2].first] = old;
    res += pair_to_string(kings[move_counter % 2]);
    return res;
}


int main() {
    std::ifstream input("../database/opening_book_notation.txt");
    std::ofstream output("../database/opening_book_moves.txt");
    std::string game;
    std::string move_notation;
    int game_number = 0;

    while (std::getline(input, game)) {
        game_number++;
        if (game_number == 20491) {
            std::cout << "Debug";
        }
        pawns = {{},
                 {}};
        for (int i = 0; i < 8; i++) {
            pawns[0].emplace_back(i, 1);
            pawns[1].emplace_back(i, 6);
        }
        rooks = {{{0, 0}, {7, 0}},
                 {{0, 7}, {7, 7}}};
        knights = {{{1, 0}, {6, 0}},
                   {{1, 7}, {6, 7}}};
        bishops = {{{2, 0}, {5, 0}},
                   {{2, 7}, {5, 7}}};
        queens = {{{3, 0}},
                  {{3, 7}}};
        kings = {{4, 0},
                 {4, 7}};
        board = {{2 * ROOK + WHITE, 2 * KNIGHT + WHITE, 2 * BISHOP + WHITE, 2 * QUEEN + WHITE, 2 * KING + WHITE,
                                                                                                                 2 *
                                                                                                                 BISHOP +
                                                                                                                 WHITE,
                                                                                                                     2 *
                                                                                                                     KNIGHT +
                                                                                                                     WHITE,
                                                                                                                         2 *
                                                                                                                         ROOK +
                                                                                                                         WHITE},
                 {2 * PAWN + WHITE, 2 * PAWN + WHITE,   2 * PAWN + WHITE,   2 * PAWN + WHITE,  2 * PAWN + WHITE, 2 *
                                                                                                                 PAWN +
                                                                                                                 WHITE,
                                                                                                                     2 *
                                                                                                                     PAWN +
                                                                                                                     WHITE,
                                                                                                                         2 *
                                                                                                                         PAWN +
                                                                                                                         WHITE},
                 {-1,               -1,                 -1,                 -1,                -1,               -1, -1, -1},
                 {-1,               -1,                 -1,                 -1,                -1,               -1, -1, -1},
                 {-1,               -1,                 -1,                 -1,                -1,               -1, -1, -1},
                 {-1,               -1,                 -1,                 -1,                -1,               -1, -1, -1},
                 {2 * PAWN + BLACK, 2 * PAWN + BLACK,   2 * PAWN + BLACK,   2 * PAWN + BLACK,  2 * PAWN + BLACK, 2 *
                                                                                                                 PAWN +
                                                                                                                 BLACK,
                                                                                                                     2 *
                                                                                                                     PAWN +
                                                                                                                     BLACK,
                                                                                                                         2 *
                                                                                                                         PAWN +
                                                                                                                         BLACK},
                 {2 * ROOK + BLACK, 2 * KNIGHT + BLACK, 2 * BISHOP + BLACK, 2 * QUEEN + BLACK, 2 * KING + BLACK, 2 *
                                                                                                                 BISHOP +
                                                                                                                 BLACK,
                                                                                                                     2 *
                                                                                                                     KNIGHT +
                                                                                                                     BLACK,
                                                                                                                         2 *
                                                                                                                         ROOK +
                                                                                                                         BLACK},
        };
        int move_counter = 0;
        char c;
        int i = 0;
        while (i < game.size() && move_counter <= NUM_OF_OPENING_MOVES * 2) {
            c = game[i];
            if (c == ' ') {
                move_counter++;
                i++;
                continue;
            }
            move_notation = "";
            while (game[i] != ' ') {
                if (game[i] != 'x' && game[i] != '+')
                    move_notation += game[i];
                i++;
            }
            if (std::islower(move_notation[0])) { // Pawn case
                output << handle_pawn(move_counter, move_notation) << " ";
                continue;
            }
            switch (move_notation[0]) {
                case 'N':
                    output << handle_knight(move_counter, move_notation);
                    break;
                case 'B':
                    output << handle_bishop(move_counter, move_notation);
                    break;
                case 'R':
                    output << handle_rook(move_counter, move_notation);
                    break;
                case 'Q':
                    output << handle_queen(move_counter, move_notation);
                    break;
                case 'K':
                    output << handle_king(move_counter, move_notation);
                    break;
                case 'O':
                    output << handle_castle(move_counter, move_notation);
                    break;
                default:
                    assert(false);
            }
            output << " ";
        }
        output << "\n";
    }
    input.close();
    output.close();
}