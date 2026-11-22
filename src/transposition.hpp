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
#include <vector>

struct TranspositionTable {

    TranspositionTable();

    enum class EntryType : u8 {
        Exact,
        Upper,
        Lower
    };  

    struct Entry {
        u64 key;
        int depth;
        Move move;
        Score score;
        EntryType type;
    };

    const Entry *probe(u64 key) const;
    void write(Entry entry);

    int hashfull() const;
    void clear();

    private:
    std::vector<Entry> data;
};  