#ifndef PIECE_ATTACK_MASKS_H
#define PIECE_ATTACK_MASKS_H

extern U64 BISHOP_LOOKUP_MASKS[64];

extern U64 ROOK_LOOKUP_MASKS[64];

extern U64 PAWN_LOOKUP_MASKS[2][64];

extern U64 KNIGHT_LOOKUP_MASKS[64];

extern U64 KING_LOOKUP_MASKS[64];


extern int BISHOP_RELEVANT_BITS[64];

extern int ROOK_RELEVANT_BITS[64];


extern void init_piece_lookup_masks();

extern void init_bishop_rook_relevant_bits();

#endif // PIECE_ATTACK_MASKS_H