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
#include <ostream>

/*
 * Move Encoding Scheme:
 * Bits 0-5             - from
 * Bits 6-11            - to
 * Bits 12-15           - kind
*/

enum class MoveKind {
    Normal,
    DoublePawnPush,
    EnPassant,
    CastlingShort,
    CastlingLong,
    PromoKnight,
    PromoBishop,
    PromoRook,
    PromoQueen,
};

struct Move {
    Move() : data(0) {}
    Move(Square from, Square to, MoveKind kind)
        : data(
            static_cast<u16>(from) | (static_cast<u16>(to) << 6) | (static_cast<u16>(kind) << 12)
        ) {}


    // Lookup functions that just return shifts and masks
    Square from() const { return static_cast<Square>(data & 0x3F); }
    Square to() const { return static_cast<Square>((data >> 6) & 0x3F); }
    MoveKind kind() const { return static_cast<MoveKind>((data >> 12) & 0xF); }

    // For null move pruning
    bool is_null() const { return data == 0; }

    u16 get_data() const { return data; }

    private:
    u16 data;    
};

// Named plain because it doesn't do move ordering
struct PlainMoveList {

    PlainMoveList() {}
    Move operator[](int i) const { return data[i]; }
    
    Move data[256];
    int size = 0;
};

// Printing functions for moves and plain move lists

inline std::ostream &operator<<(std::ostream &os, const Move &move) {
    if (move.kind() >= MoveKind::PromoKnight) {

        constexpr const char PROMO_CHARS[] = {'n', 'b', 'r', 'q'};

        os << square_to_algebraic(move.from()) << square_to_algebraic(move.to()) 
            << PROMO_CHARS[static_cast<int>(move.kind()) - static_cast<int>(MoveKind::PromoKnight)];
        return os;
    }

    os << square_to_algebraic(move.from()) << square_to_algebraic(move.to());
    return os;
}

inline std::ostream &operator << (std::ostream &os, const PlainMoveList &moves) {
    for (int i = 0; i < moves.size; ++i) {
        os << moves[i] << '\n';
    }

    os << "\nSize: " << moves.size << "\n\n";
    return os;
}