#pragma once
#include "board.hpp"
#include "move.hpp"

namespace Search {

void iterative_deepening(const Board &board, int depth, int max_ms, const RepetitionStack &stack);
void stop();

struct SearchInfo {
    RepetitionStack stack;
    u64 nodes = 0;
    Move best_move;
};

enum class NodeKind {
    Root,
    NotPrincipleVariation,
    PrincipleVariation,
};

template <NodeKind kind>
Score negamax(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta);

Score quiescence(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta);

}
