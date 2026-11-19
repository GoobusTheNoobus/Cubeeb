#include "board.hpp"
#include "bitboard.hpp"
#include "core.hpp"
#include "zobrist.hpp"
#include <cctype>
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

    // Print board
    for (int r = 7; r >= 0; --r) {
        os << r + 1 << ' ';
        for (int f = 0; f <= 7; ++f) {
            os << piece_to_char(board.board_arr[r << 3 | f]) << ' ';
        } 
        os << "\n";
    }

    os << "  a b c d e f g h\n\n";


    // Print rest of game information

    os << "  Side to move: " << (board.side_to_move == Color::White ? "White" : "Black") << '\n';
    os << "  En passant square: " << square_to_algebraic(board.ep_square) << '\n';
    os << "  Castling: ";
    os << "    WK: " << static_cast<bool>(board.castling & CASTLING_WK) << ' '; 
    os << "    WQ: " << static_cast<bool>(board.castling & CASTLING_WQ) << ' ';
    os << "    BK: " << static_cast<bool>(board.castling & CASTLING_BK) << ' ';
    os << "    BQ: " << static_cast<bool>(board.castling & CASTLING_BQ) << '\n';
    os << "  50 Move Rule: " << static_cast<int>(board.fifty_move) << "\n";

    // Zobrist hash printed in hex (base-16)
    os << "  Zobrist hash: " << std::hex << "0x" << board.zobrist << std::dec << "\n\n";
    return os; 
}

// Don't question my FEN parsing approach
// TODO: Implement zobrist hashing 
constexpr int 
    FEN_STAGE_BOARD = 0, FEN_STAGE_SIDE = 1, FEN_STAGE_CASTLING = 2,
    FEN_STAGE_EP = 3, FEN_STAGE_RULE_FIFTY = 4;
void Board::parse_fen(std::string fen) {

    // Reset all game states before parsing

    board_arr.fill(Piece::None);
    piece_bb.fill(0ULL);
    color_bb.fill(0ULL);

    side_to_move = Color::White;
    castling = 0;
    fifty_move = 0;
    ep_square = Square::None;

    zobrist = 0;

    // We use string stream to split the fen by its spaces
    std::istringstream stream(fen);
    std::string token;

    int stage = 0;
    while (stream >> token) {
        switch (stage) {
            case FEN_STAGE_BOARD: {

                int rank = 7, file = 0;
                for (char c : token) {

                    // Slash character means new line 
                    if (c == '/') {
                        file = 0;
                        --rank;
                        continue;
                    }

                    // Digit character moves file over by that number
                    if (std::isdigit(static_cast<unsigned char>(c))) {
                        file += c - '0';
                        continue;
                    }

                    // Otherwise, we see it as a piece
                    Piece piece = char_to_piece(c);
                    Square square = make_square(rank, file);
                    place_piece(square, piece);

                    ++file;
                }

                break;
            }

            case FEN_STAGE_SIDE: {
                if (token == "b") {
                    side_to_move = Color::Black;
                    zobrist ^= zobrist_black_move();
                }
                break;
            }

            case FEN_STAGE_CASTLING: {
                for (char c : token) {

                    // We look at every character in the "block," if it isn't recognized we don't count 
                    // it. Technically, - is valid for no castling, so we don't immediately assume
                    // that an unrecognized character is invalid fen
                    switch (c) {
                        case 'K': castling |= CASTLING_WK; break;
                        case 'Q': castling |= CASTLING_WQ; break;
                        case 'k': castling |= CASTLING_BK; break;
                        case 'q': castling |= CASTLING_BQ; break;
                    }
                }

                zobrist ^= zobrist_castling(castling);
                break;
            }

            case FEN_STAGE_EP: {
                ep_square = algebraic_to_square(token);
                zobrist ^= zobrist_ep(ep_square);
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


bool Board::is_square_attacked(Square square, Color by) const {

    u64 our = color_bb[static_cast<int>(by)];
    u64 occ = color_bb[0] | color_bb[1];

    if (get_pawn_attack(square, opposite(by)) & piece_bb[static_cast<int>(PieceType::Pawn)] & our)
        return true;

    if (get_knight_attack(square) & piece_bb[static_cast<int>(PieceType::Knight)] & our) 
        return true;

    if (get_bishop_attack(square, occ) & (
        piece_bb[static_cast<int>(PieceType::Bishop)] | piece_bb[static_cast<int>(PieceType::Queen)]
    ) & our)
        return true;

    if (get_rook_attack(square, occ) & (
        piece_bb[static_cast<int>(PieceType::Rook)] | piece_bb[static_cast<int>(PieceType::Queen)]
    ) & our)
        return true;  

    if (get_king_attack(square) & piece_bb[static_cast<int>(PieceType::King)] & our) 
        return true;

    return false;
}

bool Board::is_in_check(Color color) const {
    int king_sq = get_lsb(piece_bb[static_cast<int>(PieceType::King)] & color_bb[static_cast<int>(color)]);
    return is_square_attacked(static_cast<Square>(king_sq), color);
}

bool Board::is_in_check() const {
    return is_in_check(side_to_move);
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

    zobrist ^= zobrist_psq(piece, square);
}

void Board::clear_square(Square square) {

    Piece piece_there = board_arr[static_cast<int>(square)];
    if (piece_there == Piece::None) return;

    u64 mask = ~get_mask(square);
    

    board_arr[static_cast<int>(square)] = Piece::None;
    piece_bb[static_cast<int>(ptype_of(piece_there))] &= mask;
    color_bb[static_cast<int>(color_of(piece_there))] &= mask;

    zobrist ^= zobrist_psq(piece_there, square);
}