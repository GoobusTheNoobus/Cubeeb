#include "search.hpp"
#include "core.hpp"
#include "move.hpp"
#include "ordering.hpp"
#include "score.hpp"
#include "transposition.hpp"

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

static bool is_noisy(const Board &board, Move move) {

    return move.kind() == MoveKind::EnPassant || move.kind() >= MoveKind::PromoQueen ||
        board.board_arr[static_cast<int>(move.to())] != Piece::None;
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

        std::cout 
            << "info depth " << cur_depth << " score " << score_to_string(score)
            << " nodes " << info.nodes << " time " << elapsed << " nps " << (info.nodes * 1000 / elapsed)
            << " hashfull " << tt.hashfull() << " pv " << best_move << std::endl;
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
    if ((board.fifty_move == 100 || info.stack.is_repetition(board.zobrist)) && 
        kind != NodeKind::Root) {
        return DRAW_SCORE;
    }

    if (depth == 0) {
        return quiescence(info, board, ply * 2 + 2, ply + 1, alpha, beta);
    }


    auto tt_probe = tt.probe(board.zobrist);
    Move tt_ordering_move = tt_probe ? tt_probe->move : Move();
    if (tt_probe && tt_probe->depth >= depth) {
        Score tt_score = tt_probe->score;

        // Correct tt score if it is mate
        if (tt_score > MAX_CENTIPAWN) {
            tt_score -= ply;
        }

        else if (tt_score <= -MAX_CENTIPAWN) {
            tt_score += ply;
        }

        if (kind == NodeKind::NotPrincipleVariation && tt_probe->type == TranspositionTable::EntryType::Exact) {
            return tt_score;
        }

        else if (tt_probe->type == TranspositionTable::EntryType::Lower) {
            alpha = std::max(alpha, tt_score);
        }

        else if (tt_probe->type == TranspositionTable::EntryType::Upper) {
            beta = std::min(beta, tt_score);
        }

        if (kind == NodeKind::NotPrincipleVariation && alpha >= beta)
            return tt_score;
    }

    bool in_check = board.is_in_check();

    // Reverse Futility Pruning
    Score eval = board.evaluate();
    Score margin = 150 * depth;
    if (kind == NodeKind::NotPrincipleVariation && !in_check && 
        eval >= beta + margin) {
        return eval;
    }

    if (kind == NodeKind::NotPrincipleVariation && !in_check && depth >= 3 &&
        beta > eval && board.has_non_pawn_material(board.side_to_move)) {
        Board null_board = board;
        null_board.make_move(Move());

        Score score = -negamax<NodeKind::NotPrincipleVariation>(
            info, 
            null_board, 
            depth - 3, 
            ply + 1, 
            -beta, 
            -(beta - 1)
        );

        if (score >= beta)
            return score;
    }

    // Store a copy for TT flag 
    Score orig_alpha = alpha;

    OrderingMoveList moves(board, tt_ordering_move, history);

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

        Score score;
        if (kind == NodeKind::NotPrincipleVariation || move_count > 1) {
            score = -negamax<NodeKind::NotPrincipleVariation>(
                info, 
                child, 
                depth - 1, 
                ply + 1, 
                -alpha - 1, 
                -alpha
            );
        } 

        if (kind != NodeKind::NotPrincipleVariation && (move_count == 1 || score > alpha))
            score = -negamax<NodeKind::PrincipleVariation>(
                info, 
                child, 
                depth - 1, 
                ply + 1, 
                -beta, 
                -alpha
            );

        if (Timer::is_timeout()) {
            return 0;
        }

        if (score > best_score) {
            best_score = score;
            best_move = move;
        }

        alpha = std::max(alpha, score);

        if (alpha >= beta) {

            if (!is_noisy(board, move))
                history.reward(board.side_to_move, move.from(), move.to(), depth);

            for (int i = 0; i < moves.cur - 1; ++i) {
                
                Move move = moves.moves[i];
                if (!is_noisy(board, move)) {
                    Board test_board = board;
                    if (!test_board.try_move(move))
                        continue;

                    history.penalize(board.side_to_move, move.from(), move.to(), depth);
                } 
                    
            }

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



    TranspositionTable::EntryType store_flag;
    if (best_score <= orig_alpha) store_flag = TranspositionTable::EntryType::Upper;
    else if (best_score >= beta) store_flag = TranspositionTable::EntryType::Lower;
    else store_flag = TranspositionTable::EntryType::Exact;


    Score store_score = best_score;
    if (best_score > MAX_CENTIPAWN) {
        store_score += ply;
    }

    if (best_score <= -MAX_CENTIPAWN) {
        store_score -= ply;
    }

    tt.write({board.zobrist, depth, best_move, store_score, 
        store_flag});

    return best_score;
}

Score quiescence(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta) {

    if (Timer::is_timeout()) {
        return 0;
    }

    ++info.nodes;

    if (depth == 0) {
        return board.evaluate();
    }

    Score static_eval = board.evaluate();
    Score best_eval = static_eval;

    if (best_eval >= beta) 
        return best_eval;
    if (best_eval > alpha)
        alpha = best_eval;

    OrderingMoveList moves(board, Move(), history);

    info.stack.push(board.zobrist);

    std::optional<Move> opt_move;
    while ((opt_move = moves.next())) {
        Move move = opt_move.value();

        if (!is_noisy(board, move)) {
            continue;
        }

        Board child = board;
        if (!child.try_move(move)) {
            continue;
        }

        Score score = -quiescence(info, child, depth - 1, ply + 1, -beta, -alpha);

        if (Timer::is_timeout()) 
            return 0;

        if (score > best_eval)
            best_eval = score;

        alpha = std::max(alpha, score);
        if (alpha >= beta)
            break;
    }

    info.stack.pop();

    return best_eval;
}

}
