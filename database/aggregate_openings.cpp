#include <fstream>
#include <random>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <cassert>
#include <algorithm>

const int WHITE = 0;
const int BLACK = 1;

const int PAWN = 0;
const int KNIGHT = 1;
const int BISHOP = 2;
const int ROOK = 3;
const int QUEEN = 4;
const int KING = 5;

const int WP = 2 * PAWN + WHITE;
const int BP = 2 * PAWN + BLACK;
const int WN = 2 * KNIGHT + WHITE;
const int BN = 2 * KNIGHT + BLACK;
const int WB = 2 * BISHOP + WHITE;
const int BB = 2 * BISHOP + BLACK;
const int WR = 2 * ROOK + WHITE;
const int BR = 2 * ROOK + BLACK;
const int WQ = 2 * QUEEN + WHITE;
const int BQ = 2 * QUEEN + BLACK;
const int WK = 2 * KING + WHITE;
const int BK = 2 * KING + BLACK;
const int NO = -1;

std::vector<std::vector<int>> initialize() {
    return {
            {WR, WN, WB, WQ, WK, WB, WN, WR},
            {WP, WP, WP, WP, WP, WP, WP, WP},
            {NO, NO, NO, NO, NO, NO, NO, NO},
            {NO, NO, NO, NO, NO, NO, NO, NO},
            {NO, NO, NO, NO, NO, NO, NO, NO},
            {NO, NO, NO, NO, NO, NO, NO, NO},
            {BP, BP, BP, BP, BP, BP, BP, BP},
            {BR, BN, BB, BQ, BK, BB, BN, BR}
    };
}


int square(int x, int y) {
    return 8 * y + x;
}

unsigned long long hash_position(const std::vector<std::vector<int>> &board,
                                 const std::vector<std::vector<unsigned long long>> &zobrist_table) {
    unsigned long long hash = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != NO)
                hash ^= zobrist_table[square(j, i)][board[i][j]];
        }
    }
    return hash;
}

int main() {
    std::ifstream input("../database/opening_book_moves.txt");

    std::unordered_map<unsigned long long, std::unordered_map<std::string, int>> positions;
    std::mt19937_64 gen;
    std::vector<std::vector<unsigned long long>> zobrist_table(64, std::vector<unsigned long long>(12));
    for (auto &row: zobrist_table) {
        std::ranges::for_each(row.begin(), row.end(), [&gen](unsigned long long &value) {
            value = gen();
        });
    }

    std::vector<std::vector<int>> board = initialize();

    unsigned long long start_hash = hash_position(board, zobrist_table);

    std::string game_string;
    while (std::getline(input, game_string)) {
        std::stringstream game(game_string);
        std::string move;
        board = initialize();
        unsigned long long hash = start_hash;
        while (game >> move) {
            int x_from = move[0] - '0';
            int y_from = move[1] - '0';
            int x_to = move[2] - '0';
            int y_to = move[3] - '0';

            positions[hash][move]++;

            if (board[y_from][x_from] / 2 == PAWN && x_from != x_to && board[y_to][x_to] == NO) {
                hash ^= zobrist_table[square(x_to, y_from)][board[y_from][x_to]];
                board[y_from][x_to] = NO;
            }
            hash ^= zobrist_table[square(x_from, y_from)][board[y_from][x_from]];
            if (board[y_to][x_to] != NO) {
                hash ^= zobrist_table[square(x_to, y_to)][board[y_to][x_to]];
            }
            board[y_to][x_to] = board[y_from][x_from];
            board[y_from][x_from] = NO;
            hash ^= zobrist_table[square(x_to, y_to)][board[y_to][x_to]];
            if (board[y_to][x_to] / 2 == KING && abs(x_from - x_to) == 2) {
                int rook_start_pos = x_from - x_to == 2 ? 0 : 7;
                board[y_from][std::midpoint(x_to, x_from)] = board[y_from][rook_start_pos];
                board[y_from][rook_start_pos] = NO;
                hash = hash_position(board, zobrist_table);
            }
        }
    }

    input.close();
    // -----------------------------------------------------------------------------------------------------------------
    std::ofstream output("../database/opening_book.txt");
    for (const auto &row: zobrist_table) {
        for (auto value: row) {
            output << value << " ";
        }
        output << std::endl;
    }
    for (const auto &[pos, moves_and_freq]: positions) {
        output << pos << " ";
        for (auto &[move, freq]: moves_and_freq) {
            output << move << " " << freq << " ";
        }
        output << std::endl;
    }
    output.close();

    std::cout << start_hash;
}