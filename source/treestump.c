#include "engine.h"

#include "engine/piece-lookup-masks.h"
#include "engine/piece-lookup-attacks.h"

extern U64 BISHOP_LOOKUP_ATTACKS[64][512];

extern U64 ROOK_LOOKUP_ATTACKS[64][4096];

extern int rook_occupancy_index(Square square, U64 occupancy);

extern U64 create_index_occupancy(int index, U64 attackMask, int bitAmount);

extern int bitboard_bit_count(U64 bitboard);

extern U64 mask_rook_attacks(Square square);

int main(int argc, char* argv[])
{
  printf("\n[ treestump ]\n\n");

  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();

  return 0;
}