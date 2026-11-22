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
#include "move.hpp"
#include "score.hpp"
#include <array>
#include <optional>
#include <ostream>
#include <string>

inline constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
inline constexpr u8 CASTLING_WK = 1, CASTLING_WQ = 2, CASTLING_BK = 4, CASTLING_BQ = 8;
inline constexpr u8 CASTLING_WK_IDX = 0, CASTLING_WQ_IDX = 1, CASTLING_BK_IDX = 2, CASTLING_BQ_IDX = 3;

struct Board {

    // Sets up starting position
    Board(); 

    // Sets up custom position via fen
    Board(std::string fen);

    void parse_fen(std::string fen); // Invalid fen may result in undefined behavior

    bool is_square_attacked(Square square, Color color) const;
    bool is_in_check(Color color) const;
    bool is_in_check() const;

    void make_move(Move move);
    bool try_move(Move move);

    std::optional<Move> parse_move(const std::string &str) const;
    bool has_non_pawn_material(Color side) const;

    Score evaluate() const;

    // Board & pieces representation

    std::array<Piece, 64> board_arr;

    std::array<u64, 6> piece_bb;
    std::array<u64, 2> color_bb;
    
    // Game states

    Color side_to_move = Color::White;
    Square ep_square = Square::None;
    u8 fifty_move = 0;
    u8 castling = 0;

    // Engine specific states

    u64 zobrist = 0;

    Score mg_score = 0;
    Score eg_score = 0;

    private:
    void place_piece(Square square, Piece piece);
    void clear_square(Square square);
};  

std::ostream &operator << (std::ostream &os, const Board &board);

struct RepetitionStack {
    u64 data[1000];
    int ply = 0;

    void push(u64 hash) {
        data[ply++] = hash;
    }

    void pop() {
        --ply;
    }

    bool is_repetition(u64 cur) const;
};