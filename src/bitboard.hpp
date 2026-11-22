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

#pragma once
#include "core.hpp"
#include <bit>

// General purpose bitboard functions

// Finds the lease signficant bit 
inline int get_lsb(u64 bb) {
    return std::countr_zero(bb);
}

// Finds the number of set bits in bitboard
inline int popcount(u64 bb) {
    return std::popcount(bb);
}


// Helper function that pops the least significant bit and returns the index of it
inline int pop_lsb(u64& bb) {
    int lsb = get_lsb(bb);
    bb &= bb - 1; // another bit trick!
    return lsb;
}

// Returns a bitboard having a set bit at the square
inline u64 get_mask(Square square) {
    return 1ULL << static_cast<int>(square);
}

// Returns a bitboard of set bits on a certain rank
inline u64 get_rank_mask(int rank) {
    return 0xFFULL << (rank * 8);
}

// Returns a bitboard of set bits on a certain file
inline u64 get_file_mask(int file) {
    return 0x0101010101010101ULL << file;
}

// Attack bitboard functions

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

