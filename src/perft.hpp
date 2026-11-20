#pragma once
#include "board.hpp"

void perft_divide(const Board &board, int depth);
u64 perft(const Board &board, int depth);