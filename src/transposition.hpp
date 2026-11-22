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

    int hashfull();

    private:
    std::vector<Entry> data;
};  