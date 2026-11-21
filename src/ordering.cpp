#include "ordering.hpp"
#include "core.hpp"
#include "move.hpp"
#include <optional>

void OrderingMoveList::score_moves() {
    for (int i = 0; i < size; ++i) {
        scores[i] = score_of(moves[i]);
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

int OrderingMoveList::score_of(Move move) const {

    MoveKind kind = move.kind();

    if (board.board_arr[static_cast<int>(move.to())] != Piece::None) {

        PieceType moving = ptype_of(board.board_arr[static_cast<int>(move.from())]);
        PieceType captured = kind == MoveKind::EnPassant ? ptype_of(board.board_arr[static_cast<int>(move.to())]) 
            : PieceType::Pawn;

        int moving_value = MVVLVA_PIECE_VALUES[static_cast<int>(moving)];
        int capture_value = MVVLVA_PIECE_VALUES[static_cast<int>(captured)];

        int mvvlva_score = 60'000 + capture_value - moving_value / 10;
        
        return mvvlva_score;
    }

    if (kind >= MoveKind::PromoQueen) {
        return PROMO_VALUES[static_cast<int>(kind) - static_cast<int>(MoveKind::PromoQueen)];
    }

    return 0;
}

std::optional<Move> OrderingMoveList::next() {

    /*
    if (i >= size_)
        return false;

    int highestScoreIndex = i;
    int highestScore = scores[i];

    for (int j = i; j < size_; ++j) {
        if (scores[j] > highestScore) {
            highestScore = scores[j];
            highestScoreIndex = j;
        }
    }

    std::swap(moves[i], moves[highestScoreIndex]);
    std::swap(scores[i], scores[highestScoreIndex]);

    return true;
    */

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