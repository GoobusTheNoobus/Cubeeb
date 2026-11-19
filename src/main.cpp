#include "bitboard.hpp"
#include "board.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "zobrist.hpp"
#include <iostream>

int main() {
    std::cout << "Cubeeb UCI Chess Engine\n";
    init_attacks();
    init_zobrist();

    Board board;
    std::cout << board;
    
    MoveList list;
    MoveGenerator gen(list.moves, list.size);
    gen.generate_moves(board);

    std::cout << list;
}