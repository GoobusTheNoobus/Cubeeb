#pragma once
#include "core.hpp"
#include <random>

// For zobrist hashing, we use RNG values for the hash values so each position gets a basic hash.

inline u64 PIECE_SQUARE_HASH[12][64];
inline u64 CASTLING_HASH[16]; // There are 16 possible caslting rights
inline u64 EN_PASSANT_HASH[8]; // 8 Files
inline u64 BLACK_MOVE_HASH;

inline void init_zobrist() {
    std::mt19937_64 rng(1337);

    for (int i = 0; i < 12; ++i) {
        for (int sq = 0; sq < 64; ++sq) {
            PIECE_SQUARE_HASH[i][sq] = rng();
        }
    }

    // Only fill 1 -> 15
    for (int i = 1; i < 16; ++i) {
        CASTLING_HASH[i] = rng();
    }

    for (int i = 0; i < 8; ++i) {
        EN_PASSANT_HASH[i] = rng();
    }

    BLACK_MOVE_HASH = rng();
    
}

inline u64 zobrist_psq(Piece piece, Square square) { 
    return PIECE_SQUARE_HASH[static_cast<int>(piece)][static_cast<int>(square)];
} 

inline u64 zobrist_castling(u8 castling) {
    return CASTLING_HASH[castling];
}

inline u64 zobrist_ep(Square square) {
    return square == Square::None ? 0 : EN_PASSANT_HASH[static_cast<int>(square) & 7];
}

inline u64 zobrist_black_move() {
    return BLACK_MOVE_HASH;
}

