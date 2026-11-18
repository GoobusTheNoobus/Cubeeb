#pragma once
#include "core.hpp"

// General purpose bitboard functions

// Helper function that pops the least significant bit and returns the index of it
inline int pop_lsb(u64& bb) {
    int lsb = bb & -bb; // bit trick
    bb &= bb - 1; // another bit trick!
    return lsb;
}

// Returns a bitboard having a set bit at the square
inline u64 get_mask(Square square) {
    return 1ULL << static_cast<int>(square);
}

void init_attacks();

// Leapers don't care about what pieces are on the board, they cannot be blocked.
u64 get_pawn_attack(Square square, Color color);
u64 get_knight_attack(Square square);
u64 get_king_attack(Square square);

// Sliders on the other hand DO care about other pieces on the board, since they could be blocked.
u64 get_bishop_attack(Square square, u64 occ);
u64 get_rook_attack(Square square, u64 occ);

// Queen attacks are just bishop and rook combined
inline u64 get_queen_attack(Square square, u64 occ) {
    return get_bishop_attack(square, occ) | get_rook_attack(square, occ);
}