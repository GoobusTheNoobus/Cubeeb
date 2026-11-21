#pragma once
#include "board.hpp"
#include "move.hpp"

class MoveGenerator {
    public:
    MoveGenerator(Move arr[], int &size) : arr(arr), size(size) {}
    void generate_moves(const Board &board);
    
    private:

    // We store the pointer to the move array and size reference to avoid passing it 
    // through every function
    Move *arr;
    int &size;

    void generate_pawn_moves(const Board &board);
    void generate_knight_moves(const Board &board);
    void generate_bishop_moves(const Board &board);
    void generate_rook_moves(const Board &board);
    void generate_queen_moves(const Board &board);
    void generate_king_moves(const Board &board);
    void generate_castling(const Board &board);
};