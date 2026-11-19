#include "movegen.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "core.hpp"
#include "move.hpp"
#include <iostream>

void MoveGenerator::generate_moves(const Board &board) {
    generate_pawn_moves(board);
    generate_knight_moves(board);
    generate_bishop_moves(board);
    generate_rook_moves(board);
    generate_queen_moves(board);
    generate_king_moves(board);
    generate_castling(board);
}

void MoveGenerator::generate_pawn_moves(const Board &board) {

    // Set board bitboards
    u64 pawns = board.piece_bb[static_cast<int>(PieceType::Pawn)] & 
        board.color_bb[static_cast<int>(board.side_to_move)];
    u64 occ = board.color_bb[0] | board.color_bb[1];
    u64 enemy = board.color_bb[static_cast<int>(opposite(board.side_to_move))];

    bool is_white = board.side_to_move == Color::White;

    // Set offset values

    int single_push_offset = is_white ? 8 : -8;
    int double_push_offset = single_push_offset * 2;

    int left_capture_offset = is_white ? 7 : -9;
    int right_capture_offset = is_white ? 9 : -7;

    // Set mask bitboards

    u64 rank3_from_bottom = is_white ? get_rank_mask(2) : get_rank_mask(5);
    u64 rank8_from_bottom = is_white ? get_rank_mask(7) : get_rank_mask(0);
    u64 a_file = get_file_mask(0);
    u64 h_file = get_file_mask(7);

    // Finally - movement bitboards
    // The following bitboards contain the destination squares of every kind of pawn movements.

    u64 single_push_bb = (is_white ? pawns << 8 : pawns >> 8) & ~occ;
    u64 double_push_bb = (is_white ? ((single_push_bb & rank3_from_bottom) << 8) : 
        ((single_push_bb & rank3_from_bottom) >> 8)) & ~occ;
    u64 left_capture_bb = (is_white ? (pawns & ~a_file) << 7 : (pawns & ~a_file) >> 9) & enemy;
    u64 right_capture_bb = (is_white ? (pawns & ~h_file) << 9 : (pawns & ~h_file) >> 7) & enemy;

    // Promotion bitboards
    u64 single_push_promo_bb = single_push_bb & rank8_from_bottom;
    // No need for double_push_promo_bb since double pushes cannot promote
    u64 left_capture_promo_bb = left_capture_bb & rank8_from_bottom;
    u64 right_capture_promo_bb = right_capture_bb & rank8_from_bottom;

    // Remove promotion bits from original
    single_push_bb &= ~rank8_from_bottom;
    left_capture_bb &= ~rank8_from_bottom;
    right_capture_bb &= ~rank8_from_bottom;

    // Extract movement bitboards

    // Normal moves

    while (single_push_bb) {
        int lsb = pop_lsb(single_push_bb);
        arr[size++] = Move(
            static_cast<Square>(lsb - single_push_offset), 
            static_cast<Square>(lsb),
            MoveKind::Normal
        );
    }

    while (left_capture_bb) {
        int lsb = pop_lsb(left_capture_bb);
        arr[size++] = Move(
            static_cast<Square>(lsb - left_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::Normal
        );
    }

    while (right_capture_bb) {
        int lsb = pop_lsb(right_capture_bb);
        arr[size++] = Move(
            static_cast<Square>(lsb - right_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::Normal
        );
    }

    // Promotion moves

    while (single_push_promo_bb) {
        int lsb = pop_lsb(single_push_promo_bb);

        arr[size++] = Move(
            static_cast<Square>(lsb - single_push_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoKnight
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - single_push_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoBishop
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - single_push_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoRook
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - single_push_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoQueen
        );
    }

    while (left_capture_promo_bb) {
        int lsb = pop_lsb(left_capture_promo_bb);
        
        arr[size++] = Move(
            static_cast<Square>(lsb - left_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoKnight
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - left_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoBishop
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - left_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoRook
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - left_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoQueen
        );
    }

    while (right_capture_promo_bb) {
        int lsb = pop_lsb(right_capture_promo_bb);

        arr[size++] = Move(
            static_cast<Square>(lsb - right_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoKnight
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - right_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoBishop
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - right_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoRook
        );
        arr[size++] = Move(
            static_cast<Square>(lsb - right_capture_offset), 
            static_cast<Square>(lsb),
            MoveKind::PromoQueen
        );
    }

    // Special pawn moves (double push and en passant)

    while (double_push_bb) {
        int lsb = pop_lsb(double_push_bb);
        arr[size++] = Move(
            static_cast<Square>(lsb - double_push_offset), 
            static_cast<Square>(lsb),
            MoveKind::DoublePawnPush
        );
    }


    if (board.ep_square == Square::None) return;

    u64 ep_candidate_squares = get_pawn_attack(board.ep_square, opposite(board.side_to_move));
    u64 ep_pawns = ep_candidate_squares & pawns;
    

    while (ep_pawns) {
        int lsb = pop_lsb(ep_pawns);

        arr[size++] = Move(
            static_cast<Square>(lsb),
            board.ep_square,
            MoveKind::EnPassant
        );
    }

    
}   

void MoveGenerator::generate_knight_moves(const Board &board) {

    u64 our = board.color_bb[static_cast<int>(board.side_to_move)];
    u64 pieces = our & board.piece_bb[static_cast<int>(PieceType::Knight)];

    while (pieces) {

        int lsb = pop_lsb(pieces);
        Square from = static_cast<Square>(lsb);

        u64 attacks = get_knight_attack(from);
        u64 to_squares = attacks & ~our; 

        while (to_squares) {
            
            Square to = static_cast<Square>(pop_lsb(to_squares));

            arr[size++] = Move(from, to, MoveKind::Normal);
        }
    }
}

void MoveGenerator::generate_bishop_moves(const Board &board) {

    u64 our = board.color_bb[static_cast<int>(board.side_to_move)];
    u64 occ = board.color_bb[0] | board.color_bb[1];
    u64 pieces = our & board.piece_bb[static_cast<int>(PieceType::Bishop)];

    while (pieces) {

        int lsb = pop_lsb(pieces);
        Square from = static_cast<Square>(lsb);

        u64 attacks = get_bishop_attack(from, occ);
        u64 to_squares = attacks & ~our; 

        while (to_squares) {
            
            Square to = static_cast<Square>(pop_lsb(to_squares));

            arr[size++] = Move(from, to, MoveKind::Normal);
        }
    }
}

void MoveGenerator::generate_rook_moves(const Board &board) {

    u64 our = board.color_bb[static_cast<int>(board.side_to_move)];
    u64 occ = board.color_bb[0] | board.color_bb[1];
    u64 pieces = our & board.piece_bb[static_cast<int>(PieceType::Rook)];

    while (pieces) {

        int lsb = pop_lsb(pieces);
        Square from = static_cast<Square>(lsb);

        u64 attacks = get_rook_attack(from, occ);
        u64 to_squares = attacks & ~our; 

        while (to_squares) {
            
            Square to = static_cast<Square>(pop_lsb(to_squares));

            arr[size++] = Move(from, to, MoveKind::Normal);
        }
    }
}

void MoveGenerator::generate_queen_moves(const Board &board) {

    u64 our = board.color_bb[static_cast<int>(board.side_to_move)];
    u64 occ = board.color_bb[0] | board.color_bb[1];
    u64 pieces = our & board.piece_bb[static_cast<int>(PieceType::Queen)];

    while (pieces) {

        int lsb = pop_lsb(pieces);
        Square from = static_cast<Square>(lsb);

        u64 attacks = get_queen_attack(from, occ);
        u64 to_squares = attacks & ~our; 

        while (to_squares) {
            
            Square to = static_cast<Square>(pop_lsb(to_squares));

            arr[size++] = Move(from, to, MoveKind::Normal);
        }
    }
}

void MoveGenerator::generate_king_moves(const Board &board) {

    u64 our = board.color_bb[static_cast<int>(board.side_to_move)];
    u64 pieces = our & board.piece_bb[static_cast<int>(PieceType::King)];

    while (pieces) {

        int lsb = pop_lsb(pieces);
        Square from = static_cast<Square>(lsb);

        u64 attacks = get_king_attack(from);
        u64 to_squares = attacks & ~our; 

        while (to_squares) {
            
            Square to = static_cast<Square>(pop_lsb(to_squares));

            arr[size++] = Move(from, to, MoveKind::Normal);
        }
    }
}

void MoveGenerator::generate_castling(const Board &board) {

    if (board.is_in_check()) return;

    bool is_white = board.side_to_move == Color::White;
    Color them = opposite(board.side_to_move);

    // Define all "in between" squares (between the castling king and rook)

    // Short castle
    Square short_between_sq1 = is_white ? Square::G1 : Square::G8;
    Square short_between_sq2 = is_white ? Square::F1 : Square::F8;

    // Long castle
    Square long_between_sq1 = is_white ? Square::C1 : Square::C8;
    Square long_between_sq2 = is_white ? Square::D1 : Square::D8;

    // We don't need to check this square for being attacked, since the king doesn't go through it. We only check occupancy
    Square long_between_sq3 = is_white ? Square::B1 : Square::B8;


    u8 short_castling_right = is_white ? CASTLING_WK : CASTLING_BK;
    u8 long_castling_right = is_white ? CASTLING_WQ : CASTLING_BQ;


    Square from = is_white ? Square::E1 : Square::E8;
    Square to_short = is_white ? Square::G1 : Square::G8;
    Square to_long = is_white ? Square::C1 : Square::C8;
    
    // Short castling

    if (
        (board.castling & short_castling_right) &&
        board.board_arr[static_cast<int>(short_between_sq1)] == Piece::None &&
        board.board_arr[static_cast<int>(short_between_sq2)] == Piece::None &&
        !board.is_square_attacked(short_between_sq1, them) &&
        !board.is_square_attacked(short_between_sq2, them)
    ) {
        arr[size++] = Move(from, to_short, MoveKind::CastlingShort);
    }


    // Long castling

    if (
        (board.castling & long_castling_right) &&
        board.board_arr[static_cast<int>(long_between_sq1)] == Piece::None &&
        board.board_arr[static_cast<int>(long_between_sq2)] == Piece::None &&
        board.board_arr[static_cast<int>(long_between_sq3)] == Piece::None &&
        !board.is_square_attacked(long_between_sq1, them) &&
        !board.is_square_attacked(long_between_sq2, them)
    ) {
        arr[size++] = Move(from, to_long, MoveKind::CastlingLong);
    }
}
