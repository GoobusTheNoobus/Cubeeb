#include "bitboard.hpp"
#include "score.hpp"
#include "uci.hpp"
#include "zobrist.hpp"
#include <iostream>

int main() {
    std::cout << "Cubeeb UCI Chess Engine\n";
    init_attacks();
    init_zobrist();
    init_psqt();

    start_uci();
}