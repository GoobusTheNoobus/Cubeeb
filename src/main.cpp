#include "bitboard.hpp"
#include "board.hpp"
#include <iostream>

int main() {
    std::cout << "Cubeeb UCI Chess Engine\n";
    init_attacks();

    Board board;
    std::cout << board;
}