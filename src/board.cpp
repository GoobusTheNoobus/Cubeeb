#include "board.hpp"
#include "bitboard.hpp"
#include "core.hpp"
#include <cctype>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

Board::Board() {
    parse_fen(START_FEN);
}

Board::Board(std::string fen) {
    parse_fen(std::move(fen));
}

std::ostream &operator << (std::ostream &os, const Board &board) {
    for (int r = 7; r >= 0; --r) {
        os << r + 1 << ' ';
        for (int f = 0; f <= 7; ++f) {
            os << piece_to_char(board.board_arr[r << 3 | f]) << ' ';
        } 
        os << "\n";
    }

    os << "  a b c d e f g h\n";
    return os;
}

// Don't question my FEN parsing approach
// TODO: Implement zobrist hashing 
constexpr int 
    FEN_STAGE_BOARD = 0, FEN_STAGE_SIDE = 1, FEN_STAGE_CASTLING = 2,
    FEN_STAGE_EP = 3, FEN_STAGE_RULE_FIFTY = 4;
void Board::parse_fen(std::string fen) {

    board_arr.fill(Piece::None);
    piece_bb.fill(0ULL);
    color_bb.fill(0ULL);

    side_to_move = Color::White;
    castling = 0;
    fifty_move = 0;
    ep_square = Square::None;

    std::istringstream stream(fen);
    std::string token;

    int stage = 0;
    while (stream >> token) {
        switch (stage) {
            case FEN_STAGE_BOARD: {

                int rank = 7, file = 0;
                for (char c : token) {

                    if (c == '/') {
                        file = 0;
                        --rank;
                        continue;
                    }

                    if (std::isdigit(static_cast<unsigned char>(c))) {
                        file += c - '0';
                        continue;
                    }

                    Piece piece = char_to_piece(c);
                    Square square = make_square(rank, file);
                    place_piece(square, piece);

                    ++file;
                }

                break;
            }

            case FEN_STAGE_SIDE: {
                side_to_move = token == "b" ? Color::Black : Color::White;
                break;
            }

            case FEN_STAGE_CASTLING: {
                for (char c : token) {
                    switch (c) {
                        case 'K': castling |= CASTLING_WK; break;
                        case 'Q': castling |= CASTLING_WQ; break;
                        case 'k': castling |= CASTLING_BK; break;
                        case 'q': castling |= CASTLING_BQ; break;
                    }
                }

                break;
            }

            case FEN_STAGE_EP: {
                ep_square = algebraic_to_square(token);
                break;
            }

            case FEN_STAGE_RULE_FIFTY: {
                fifty_move = std::stoi(token);
                break;
            }
        }

        ++stage;
    }
}

void Board::place_piece(Square square, Piece piece) {
    if (piece == Piece::None) {
        clear_square(square);
        return;
    }

    u64 mask = get_mask(square);

    board_arr[static_cast<int>(square)] = piece;
    piece_bb[static_cast<int>(ptype_of(piece))] |= mask;
    color_bb[static_cast<int>(color_of(piece))] |= mask;
}

void Board::clear_square(Square square) {

    Piece piece_there = board_arr[static_cast<int>(square)];
    if (piece_there == Piece::None) return;

    u64 mask = ~get_mask(square);
    

    board_arr[static_cast<int>(square)] = Piece::None;
    piece_bb[static_cast<int>(ptype_of(piece_there))] &= mask;
    color_bb[static_cast<int>(color_of(piece_there))] &= mask;
}