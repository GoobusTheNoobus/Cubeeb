#include "bitboard.hpp"
#include "board.hpp"
#include "zobrist.hpp"
#include <iostream>

int main() {
    std::cout << "Cubeeb UCI Chess Engine\n";
    init_attacks();
    init_zobrist();

    Board board;
    std::cout << board;
}