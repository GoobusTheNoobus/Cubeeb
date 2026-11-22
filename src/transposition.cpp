#include "transposition.hpp"
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
