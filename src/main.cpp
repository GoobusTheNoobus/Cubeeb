#include "bitboard.hpp"
#include "board.hpp"
#include "score.hpp"
#include "search.hpp"
#include "zobrist.hpp"
#include <iostream>

int main() {
    std::cout << "Cubeeb UCI Chess Engine\n";
    init_attacks();
    init_zobrist();
    init_psqt();

    Board board1("r1bqkb1r/pppp1ppp/2n2n2/4p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 4 4");
    Board board2("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    Board board3("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    iterative_deepening(board1, 4);
}