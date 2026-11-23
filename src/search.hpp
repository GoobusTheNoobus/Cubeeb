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
#include "killer.hpp"
#include "move.hpp"
#include "transposition.hpp"
#include "history.hpp"

namespace Search {

inline TranspositionTable tt;
inline History history;

void iterative_deepening(const Board &board, int depth, int max_ms, const RepetitionStack &stack);
void stop();

struct SearchInfo {
    Killer killer;
    RepetitionStack stack;
    u64 nodes = 0;
    Move best_move;

    Move pv[100][100];
    int pv_len[100];
};

enum class NodeKind {
    Root,
    NotPrincipleVariation,
    PrincipleVariation,
};

template <NodeKind kind>
Score negamax(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta);

Score quiescence(SearchInfo &info, const Board &board, int depth, int ply, Score alpha, Score beta);

}
