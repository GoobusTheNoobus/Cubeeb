#include "bitboard.hpp"
#include "board.hpp"
#include "score.hpp"
#include "zobrist.hpp"
#include <iostream>

int main() {
    std::cout << "Cubeeb UCI Chess Engine\n";
    init_attacks();
    init_zobrist();
    init_psqt();

    Board board1(START_FEN);
    Board board2("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    Board board3("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    std::cout << board1 << board2 << board3;
}