#include "perft.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <format>

void perft_divide(const Board &board, int depth) {

    PlainMoveList moves;
    MoveGenerator gen(moves.data, moves.size);
    gen.generate_moves(board);

    std::cout << "Performing performance test...\n";

    u64 nodes = 0;

    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < moves.size; ++i) {

        Move move = moves[i];

        Board child(board);
        if (!child.try_move(move))
            continue;

        std::cout << "Move #" << std::format("{:03}", i + 1) << " \"" << move << "\" - ";
        
        u64 child_nodes = perft(child, depth - 1);

        std::cout << child_nodes << '\n';

        nodes += child_nodes;
    }

    auto end_time = std::chrono::steady_clock::now();

    double ms = std::max<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count(), 1);

    std::cout << "\n================ PERFT SUMMARY ================\n";
    std::cout << "Depth:           " << depth << '\n';
    std::cout << "Total Moves:     " << static_cast<int>(moves.size) << "\n";
    std::cout << "Total Nodes:     " << nodes << "\n";
    std::cout << "Time Elapsed:    " << ms << "ms\n";
    std::cout << "NPS:             " << std::format("{:.2f}M", nodes / ms / 1000) << "\n";
    std::cout << "Position:\n\n";
    std::cout << board;
    std::cout << "===============================================\n";
}

u64 perft(const Board &board, int depth) {

    if (depth == 0) {
        return 1;
    }

    PlainMoveList moves;
    MoveGenerator gen(moves.data, moves.size);
    gen.generate_moves(board);

    u64 nodes = 0;

    for (int i = 0; i < moves.size; ++i) {

        Move move = moves[i];

        Board child(board);
        if (!child.try_move(move)) {
            continue;
        }

        u64 child_nodes = perft(child, depth - 1);
        nodes += child_nodes;
    } 

    return nodes;
}