#include "search.hpp"
#include "move.hpp"
#include "ordering.hpp"
#include "score.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <optional>

namespace Search {

namespace Timer {

int max_ms;
std::chrono::steady_clock::time_point start;
std::atomic_bool stop = false;

int elapsed() {
    auto now = std::chrono::steady_clock::now();
    return std::max<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count(), 1);
}

bool is_timeout() {

    int ms = elapsed();
    return stop || (max_ms > 0 && ms > max_ms);
}

}

void stop() { Timer::stop = true; }

void iterative_deepening(const Board &board, int depth, int max_ms, const RepetitionStack &stack) {

    if (depth == 0) {
        std::cout << "bestmove [none]\n";
        return;
    }

    Timer::max_ms = max_ms;
    Timer::start = std::chrono::steady_clock::now();
    Timer::stop = false;

    SearchInfo info { stack, 0, Move() };
    Move best_move;

    for (int cur_depth = 1; cur_depth <= depth; ++cur_depth) {

        if (Timer::is_timeout()) {
            break;
        }
        
        Score score = negamax<NodeKind::Root>(info, board, cur_depth, 0, -INF_SCORE, INF_SCORE);

        if (Timer::is_timeout()) {
            break;
        }

        best_move = info.best_move;

        int elapsed = Timer::elapsed();

        std::cout << "info depth " << cur_depth << " score " << score_to_string(score) << " pv " << best_move 
            << " nodes " << info.nodes << " time " << elapsed << " nps " << (info.nodes * 1000 / elapsed)
            << std::endl;
    }

    std::cout << "bestmove " << best_move << std::endl;
}

template <NodeKind kind>
Score negamax(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta) {
    ++info.nodes;

    if (Timer::is_timeout()) {
        return 0;
    }

    // We check for 3-fold repetition, but only if we are not at a root node, since then it would
    // not even search a single depth
    if (info.stack.is_repetition(board.zobrist) && kind != NodeKind::Root) {
        return DRAW_SCORE;
    }

    if (depth == 0) {
        return board.evaluate();
    }

    OrderingMoveList moves(board);

    Score best_score = -INF_SCORE;
    Move best_move;

    info.stack.push(board.zobrist);

    int move_count = 0;

    std::optional<Move> optmove;
    while ((optmove = moves.next())) {
        Move move = *optmove;

        Board child = board;
        if (!child.try_move(move)) {
            continue;
        }

        ++move_count;

        Score score = -negamax<NodeKind::NotRoot>(info, child, depth - 1, ply + 1, -beta, -alpha);

        if (Timer::is_timeout()) {
            return 0;
        }

        if (score > best_score) {
            best_score = score;
            best_move = move;
        }

        alpha = std::max(alpha, score);

        if (alpha >= beta) {
            break;
        }
    }

    info.stack.pop();

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

}
