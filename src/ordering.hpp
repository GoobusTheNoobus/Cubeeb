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
#include "board.hpp"
#include "history.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include <optional>

struct OrderingMoveList {
    OrderingMoveList(const Board &board, Move tt_move, const History &history) 
        : board(board) {
        MoveGenerator gen(moves, size);
        gen.generate_moves(board);
        score_moves(tt_move, history);
    }

    std::optional<Move> next();

    const Board &board;

    Move moves[256];

    // Scored on how likely they are to be good moves
    int scores[256]; 
    int size = 0;

    // to iterate through the list in scores order
    int cur = 0;

    private:
    void score_moves(Move tt_move, const History &history);
    int score_of(Move move, Move tt_move, const History &history) const;
};