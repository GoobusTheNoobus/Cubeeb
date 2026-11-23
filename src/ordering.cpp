/*
 * ========================================================================
 *
 *        /$$$$$$            /$$                           /$$      
 *       /$$__  $$          | $$                          | $$      
 *      | $$  \__/ /$$   /$$| $$$$$$$   /$$$$$$   /$$$$$$ | $$$$$$$ 
 *      | $$      | $$  | $$| $$__  $$ /$$__  $$ /$$__  $$| $$__  $$
 *      | $$      | $$  | $$| $$  \ $$| $$$$$$$$| $$$$$$$$| $$  \ $$
 *      | $$    $$| $$  | $$| $$  | $$| $$_____/| $$_____/| $$  | $$
 *      |  $$$$$$/|  $$$$$$/| $$$$$$$/|  $$$$$$$|  $$$$$$$| $$$$$$$/
 *      \______/  \______/ |_______/  \_______/ \_______/|_______/ 
 *
 * ========================================================================
 * Cubeeb is a UCI chess engine written in C++20.
 * 
 * Copyright (C) 2026  GoobusTheNoobus, picowso. 
 * All rights reserved.
 */

#include "ordering.hpp"
#include "core.hpp"
#include "move.hpp"
#include <optional>

void OrderingMoveList::score_moves(Move tt_move, const History &history) {
    for (int i = 0; i < size; ++i) {
        scores[i] = score_of(moves[i], tt_move, history);
    }
}

// We have an ordering piece value seperate from evaluation
constexpr int MVVLVA_PIECE_VALUES[] = { 100, 300, 300, 500, 900, 1200 }; 

constexpr int PROMO_VALUES[] = { 
    800,        // queen
    400,        // rooks
    200,        // bishops
    200,        // knights
};

int OrderingMoveList::score_of(Move move, Move tt_move, const History &history) const {

    // TT move is ordered first.
    if (tt_move.get_data() == move.get_data()) {
        return 100'000;
    }

    MoveKind kind = move.kind();

    // Captures are ordered next.
    if (board.board_arr[static_cast<int>(move.to())] != Piece::None) {

        PieceType moving = ptype_of(board.board_arr[static_cast<int>(move.from())]);
        PieceType captured = kind == MoveKind::EnPassant ? ptype_of(board.board_arr[static_cast<int>(move.to())]) 
            : PieceType::Pawn;

        int moving_value = MVVLVA_PIECE_VALUES[static_cast<int>(moving)];
        int capture_value = MVVLVA_PIECE_VALUES[static_cast<int>(captured)];

        int mvvlva_score = 60'000 + capture_value - moving_value / 10;
        
        return mvvlva_score;
    }

    // Promotion moves are ordered next.
    if (kind >= MoveKind::PromoQueen) {
        return PROMO_VALUES[static_cast<int>(kind) - static_cast<int>(MoveKind::PromoQueen)];
    }

    // Finally, we return its history values - historically good moves tend to be ordered first
    return history.data[static_cast<int>(board.side_to_move)]
        [static_cast<int>(move.from())][static_cast<int>(move.to())];
}

// We don't immediately sort the list - instead we sort them one by one
std::optional<Move> OrderingMoveList::next() {

    if (cur >= size) {
        return std::nullopt;
    }

    int highest_idx = cur;
    int highest = scores[cur];

    for (int i = cur; i < size; ++i) {
        if (scores[i] > highest) {
            highest = scores[i];
            highest_idx = i;
        }
    }

    std::swap(scores[highest_idx], scores[cur]);
    std::swap(moves[highest_idx], moves[cur]);

    return moves[cur++];
}