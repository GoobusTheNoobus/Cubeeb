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

// Constants

inline constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
inline constexpr u8 CASTLING_WK = 1, CASTLING_WQ = 2, CASTLING_BK = 4, CASTLING_BQ = 8;
inline constexpr u8 CASTLING_WK_IDX = 0, CASTLING_WQ_IDX = 1, CASTLING_BK_IDX = 2, CASTLING_BQ_IDX = 3;

// Board representation

struct Board {

    // Sets up empty position
    Board(); 

    // Sets up custom position via fen
    Board(std::string fen);

    void parse_fen(std::string fen); // Invalid fen may result in undefined behavior

    // Attack detections
    bool is_square_attacked(Square square, Color color) const;

    // Detects whether a certain colour's king is attacked
    bool is_in_check(Color color) const;

    // Detects whether the side to move's king is attacked
    bool is_in_check() const;

    void make_move(Move move);
    bool try_move(Move move);

    // Turns a string algebraic notation move into an internal represented Move
    std::optional<Move> parse_move(const std::string &str) const;

    // For null move pruning
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

    // Piece-square scores

    Score mg_score = 0;
    Score eg_score = 0;

    private:

    // Places a piece on a square, given that the square is empty
    void place_piece(Square square, Piece piece);

    // Clears a square, doesn't matter if it is already empty
    void clear_square(Square square);
};  

// Printing support for the board
std::ostream &operator<<(std::ostream &os, const Board &board);

// For tracking 3-fold repetitions
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