#include "search.hpp"
#include "move.hpp"
#include "score.hpp"
#include "movegen.hpp"

#include <iostream>

void iterative_deepening(const Board &board, int depth) {

    if (depth == 0) {
        std::cout << "bestmove [none]\n";
        return;
    }

    SearchInfo info;

    for (int cur_depth = 1; cur_depth <= depth; ++cur_depth) {
        
        Score score = negamax<NodeKind::Root>(info, board, cur_depth, 0, -INF_SCORE, INF_SCORE);

        std::cout << "info depth " << cur_depth << " score " << score << " pv " << info.best_move << std::endl;
    }

    std::cout << "bestmove " << info.best_move << '\n';
}

template <NodeKind kind>
Score negamax(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta) {
    ++info.nodes;

    if (depth == 0) {
        return board.evaluate();
    }

    MoveList moves;
    MoveGenerator gen(moves.data, moves.size);
    gen.generate_moves(board);

    Score best_score = -INF_SCORE;
    Move best_move;

    int move_count = 0;

    for (int i = 0; i < moves.size; ++i) {

        Move move = moves[i];

        Board child = board;
        if (!child.try_move(move)) {
            continue;
        }

        ++move_count;

        Score score = -negamax<NodeKind::NotRoot>(info, child, depth - 1, ply + 1, -beta, -alpha);

        if (score > best_score) {
            best_score = score;
            best_move = move;
        }

        alpha = std::max(alpha, score);

        if (alpha >= beta) {
            break;
        }
    }

    if (move_count == 0) {
        if (board.is_in_check()) {
            return -MATE_SCORE + 1 + ply;
        }

        return DRAW_SCORE;
    }

    if (kind == NodeKind::Root) {
        info.best_move = best_move;
    }

    return best_score;
}