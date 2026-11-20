#pragma once
#include "board.hpp"
#include "move.hpp"

void iterative_deepening(const Board &board, int depth);

struct SearchInfo {
    u64 nodes = 0;
    Move best_move;
};

enum class NodeKind {
    Root,
    NotRoot,
};

template <NodeKind kind>
Score negamax(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta);