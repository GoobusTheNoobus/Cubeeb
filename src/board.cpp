#include "board.hpp"
#include "bitboard.hpp"
#include "core.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "score.hpp"
#include "zobrist.hpp"
#include <cctype>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

Board::Board() {
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
    os << "  Zobrist hash: " << std::hex << "0x" << board.zobrist << std::dec << "\n";
    os << "  Middlegame PSQT score: " << board.mg_score << '\n';
    os << "  Endgame PSQT score: " << board.eg_score << "\n\n";
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

    mg_score = 0;
    eg_score = 0;

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
    return is_square_attacked(static_cast<Square>(king_sq), opposite(color));
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
    Color color = color_of(piece);

    board_arr[static_cast<int>(square)] = piece;
    piece_bb[static_cast<int>(ptype_of(piece))] |= mask;
    color_bb[static_cast<int>(color)] |= mask;

    zobrist ^= zobrist_psq(piece, square);

    Score mg_piece_value = psqt_middlegame_score(square, piece);
    Score eg_piece_value = psqt_endgame_score(square, piece);

    mg_score += color == Color::White ? mg_piece_value : -mg_piece_value;
    eg_score += color == Color::White ? eg_piece_value : -eg_piece_value;
}

void Board::clear_square(Square square) {

    Piece piece_there = board_arr[static_cast<int>(square)];
    if (piece_there == Piece::None) return;

    u64 mask = ~get_mask(square);

    Color color = color_of(piece_there);
    

    board_arr[static_cast<int>(square)] = Piece::None;
    piece_bb[static_cast<int>(ptype_of(piece_there))] &= mask;
    color_bb[static_cast<int>(color)] &= mask;

    zobrist ^= zobrist_psq(piece_there, square);

    Score mg_piece_value = psqt_middlegame_score(square, piece_there);
    Score eg_piece_value = psqt_endgame_score(square, piece_there);

    mg_score -= color == Color::White ? mg_piece_value : -mg_piece_value;
    eg_score -= color == Color::White ? eg_piece_value : -eg_piece_value;
}

void Board::make_move(Move move) {

    Square from = move.from();
    Square to = move.to();
    MoveKind kind = move.kind();

    Color us = side_to_move;

    Piece piece_moving = board_arr[static_cast<int>(from)];
    Piece piece_captured = kind == MoveKind::EnPassant ? make_piece(PieceType::Pawn, opposite(side_to_move)) : board_arr[static_cast<int>(to)];


    // We reset ep_square because almost always a move resets it. If it doesn't (double pawn push), it will restore.

    zobrist ^= zobrist_ep(ep_square);
    ep_square = Square::None;

    // Switch turn
    side_to_move = opposite(side_to_move);
    zobrist ^= zobrist_black_move();

    switch (kind) {
        case MoveKind::Normal: {
            clear_square(from);
            clear_square(to);
            place_piece(to, piece_moving);

            break;
        }

        case MoveKind::DoublePawnPush: {
            clear_square(from);
            place_piece(to, piece_moving);

            ep_square = static_cast<Square>(static_cast<int>(to) - (us == Color::White ? 8 : -8));
            zobrist ^= zobrist_ep(ep_square);
            break;
        }

        case MoveKind::EnPassant: {
            clear_square(from);
            place_piece(to, piece_moving);

            // The captured pawn's square is actually behind
            Square capture_square = static_cast<Square>(static_cast<int>(to) - (us == Color::White ? 8 : -8));

            clear_square(capture_square);

            break;
        }

        case MoveKind::CastlingShort:
        case MoveKind::CastlingLong: {

            bool is_castling_long = kind == MoveKind::CastlingLong;
            bool is_white = us == Color::White;
            
            Square rook_from = is_castling_long ? 
                (is_white ? Square::A1 : Square::A8) : (is_white ? Square::H1 : Square::H8);

            Square rook_to = is_castling_long ?
                (is_white ? Square::D1 : Square::D8) : (is_white ? Square::F1 : Square::F8);

            Piece rook = make_piece(PieceType::Rook, us);

            clear_square(from);
            clear_square(rook_from);

            place_piece(to, piece_moving);
            place_piece(rook_to, rook );

            break;
        }

        // Assume promotion
        default: {
            PieceType promoted_ptype = 
                static_cast<PieceType>((static_cast<int>(kind) - static_cast<int>(MoveKind::PromoKnight)) + static_cast<int>(PieceType::Knight));
            Piece promoted_piece = make_piece(promoted_ptype, us);

            clear_square(from);
            clear_square(to);
            place_piece(to, promoted_piece);

            break;
        }
    }

    // Handle castling rights
    // Use bit tricks
    // Basically, castling rights can be removed in 2 different ways:
    // 1. Rook move
    // 2. Rook get captured
    // 3. King move
    // In the following section, we handle the first 2 by using an array of masks

    constexpr u8 MASKS[64] = {
        13, 15, 15, 15, 15, 15, 15, 14,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
         7, 15, 15, 15, 15, 15, 15, 11,
    };

    zobrist ^= zobrist_castling(castling);

    castling &= MASKS[static_cast<int>(from)];
    castling &= MASKS[static_cast<int>(to)];

    // Now we handle king moves
    if (from == Square::E1) {
        castling &= ~CASTLING_WK;
        castling &= ~CASTLING_WQ;
    }

    else if (from == Square::E8) {
        castling &= ~CASTLING_BK;
        castling &= ~CASTLING_BQ;
    }

    zobrist ^= zobrist_castling(castling);

    if (piece_captured == Piece::None && ptype_of(piece_moving) != PieceType::Pawn) {
        ++fifty_move;
    } else fifty_move = 0;
}

bool Board::try_move(Move move) {
    make_move(move);

    // Ileegal: puts moving side in check
    return !is_in_check(opposite(side_to_move));
}

Score Board::evaluate() const {
    int phase = game_phase(piece_bb);

    Score material_score = (mg_score * phase + eg_score * (MAX_PHASE - phase)) / MAX_PHASE;
    Score side_score = side_to_move == Color::White ? material_score : -material_score;

    return side_score + TEMPO_BONUS;
}

std::optional<Move> Board::parse_move(const std::string &str) const {
    MoveList moves;
    MoveGenerator gen(moves.data, moves.size);
    gen.generate_moves(*this);

    for (int i = 0; i < moves.size; ++i) {
        Move move = moves[i];

        std::ostringstream oss;
        oss << move;

        if (oss.str() == str) 
            return move;

    }

    return std::nullopt;
}
