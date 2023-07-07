#include "../treestump.h"

extern int board_ls1b_index(U64 bitboard);

extern U64 random_U64(void);

extern U32 RANDOM_STATE;


U64 PIECE_HASH_KEYS[12][BOARD_SQUARES];

U64 PASSANT_HASH_KEYS[BOARD_SQUARES];

U64 CASTLE_HASH_KEYS[16];

U64 SIDE_HASH_KEY;

void init_random_keys()
{
  RANDOM_STATE = 1804289383;

  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    for(Square square = 0; square < BOARD_SQUARES; square++)
    {
      PIECE_HASH_KEYS[piece][square] = random_U64();
    }
  }
  for(Square square = 0; square < BOARD_SQUARES; square++)
  {
    PASSANT_HASH_KEYS[square] = random_U64();
  }
  for(int index = 0; index < 16; index++)
  {
    CASTLE_HASH_KEYS[index] = random_U64();
  }
  SIDE_HASH_KEY = random_U64();
}

U64 create_hash_key(Position position)
{
  U64 hashKey = 0ULL;

  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    U64 bitboard = position.boards[piece];

    while(bitboard)
    {
      Square square = board_ls1b_index(bitboard);

      hashKey ^= PIECE_HASH_KEYS[piece][square];

      bitboard = BOARD_SQUARE_POP(bitboard, square);
    }
  }

  if(position.passant != SQUARE_NONE) hashKey ^= PASSANT_HASH_KEYS[position.passant];
  
  hashKey ^= CASTLE_HASH_KEYS[position.castle];

  if(position.side == SIDE_BLACK) hashKey ^= SIDE_HASH_KEY;

  return hashKey;
}