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
#include <algorithm>

struct History {
    int data[2][64][64]{};
    static constexpr int MAX_HISTORY = 50000;

    void reward(Color side, Square from, Square to, int depth) {

        int bonus = 300 * depth - 250;
        int clamped = std::clamp(bonus, -MAX_HISTORY, MAX_HISTORY);

        int *entry = &data[static_cast<int>(side)][static_cast<int>(from)][static_cast<int>(to)];
        *entry += clamped - *entry * std::abs(clamped) / MAX_HISTORY;
    }

    void penalize(Color side, Square from, Square to, int depth) {

        int penalty = 300 * depth - 250;
        int clamped = std::clamp(penalty, -MAX_HISTORY, MAX_HISTORY);

        int *entry = &data[static_cast<int>(side)][static_cast<int>(from)][static_cast<int>(to)];
        *entry += -clamped - *entry * std::abs(clamped) / MAX_HISTORY;
    }

    void clear() {
        for (int f = 0; f < 64; ++f) {
            for (int t = 0; t < 64; ++t) {
                data[0][f][t] = 0;
                data[1][f][t] = 0;
            }
        }
    }
};