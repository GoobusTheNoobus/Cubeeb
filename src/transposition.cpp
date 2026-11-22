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

#include "transposition.hpp"
#include <algorithm>
#include <cstddef>

TranspositionTable::TranspositionTable() {
    size_t mb = 64;
    size_t entries = mb * 1024 * 1024 / sizeof(Entry);
    data.resize(entries);
}

auto TranspositionTable::probe(u64 key) const -> const Entry* {
    size_t index = key % data.size();

    const Entry &entry = data[index];
    if (entry.key == key) return &entry;
    return nullptr;
}

void TranspositionTable::write(Entry entry) {
    size_t index = entry.key % data.size();
    Entry &old = data[index];

    if (entry.depth >= old.depth || entry.key != old.key) {
        old = std::move(entry);
    }
}

int TranspositionTable::hashfull() const {
    int c = 0;
    for (int i = 0; i < 1000; ++i) {
        if (data[i].depth > 0) 
            ++c;
    }

    return c;
}

void TranspositionTable::clear() {
    std::fill(data.data(), data.data() + data.size(), Entry{});
}