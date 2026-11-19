#pragma once
#include <cassert>
#include <cstdint>
#include <string>

using u64 = uint64_t;
using u8 = uint8_t;

enum class Square : u8 {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    None,
};

enum class PieceType : u8 {
    Pawn, Knight, Bishop, Rook, Queen, King
};

enum class Color : u8 {
    White, Black
};

enum class Piece : u8 {
    WPawn, WKnight, WBishop, WRook, WQueen, WKing,
    BPawn, BKnight, BBishop, BRook, BQueen, BKing,
    None,
};

constexpr Square make_square(int r, int f) {
    return static_cast<Square>(r << 3 | f);
}

constexpr int file_of(Square sq) {
    return static_cast<int>(sq) & 7;
} 

constexpr int rank_of(Square sq) {
    return static_cast<int>(sq) >> 3;
}

constexpr PieceType ptype_of(Piece piece) {
    return static_cast<PieceType>(static_cast<int>(piece) % 6);
}

constexpr Color color_of(Piece piece) {
    return static_cast<Color>(static_cast<int>(piece) / 6);
}

constexpr Piece make_piece(PieceType pt, Color c) {
    return static_cast<Piece>(static_cast<int>(c) * 6 + static_cast<int>(pt));
}

inline Piece char_to_piece(char c) {
    switch (c) {
        case 'P': return Piece::WPawn;
        case 'N': return Piece::WKnight;
        case 'B': return Piece::WBishop;
        case 'R': return Piece::WRook;
        case 'Q': return Piece::WQueen;
        case 'K': return Piece::WKing;
        case 'p': return Piece::BPawn;
        case 'n': return Piece::BKnight;
        case 'b': return Piece::BBishop;
        case 'r': return Piece::BRook;
        case 'q': return Piece::BQueen;
        case 'k': return Piece::BKing;
        default: return Piece::None;
    }
}

inline char piece_to_char(Piece piece) {
    switch (piece) {
        case Piece::WPawn: return 'P';
        case Piece::WKnight: return 'N';
        case Piece::WBishop: return 'B';
        case Piece::WRook: return 'R';
        case Piece::WQueen: return 'Q';
        case Piece::WKing: return 'K';
        case Piece::BPawn: return 'p';
        case Piece::BKnight: return 'n';
        case Piece::BBishop: return 'b';
        case Piece::BRook: return 'r';
        case Piece::BQueen: return 'q';
        case Piece::BKing: return 'k';
        default: return '.';
    }
}

inline Square algebraic_to_square(const std::string &alg) {
    if (alg == "-") return Square::None;
    assert(alg.size() == 2);

    int rank = alg[1] - '1';
    int file = alg[0] - 'a';

    return make_square(rank, file);
}

inline std::string square_to_algebraic(Square sq) {
    if (sq == Square::None) {
        return "-";
    }

    char rank = rank_of(sq) + '1';
    char file = file_of(sq) + 'a';

    return std::string{file, rank};
}