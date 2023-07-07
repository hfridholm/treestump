#include "../treestump.h"

extern U64 BISHOP_LOOKUP_MASKS[BOARD_SQUARES];

extern U64 ROOK_LOOKUP_MASKS[BOARD_SQUARES];

extern U64 PAWN_LOOKUP_MASKS[2][BOARD_SQUARES];

extern U64 KNIGHT_LOOKUP_MASKS[BOARD_SQUARES];

extern U64 KING_LOOKUP_MASKS[BOARD_SQUARES];


extern int BISHOP_RELEVANT_BITS[BOARD_SQUARES];

extern int ROOK_RELEVANT_BITS[BOARD_SQUARES];


extern const U64 BISHOP_MAGIC_NUMBERS[64];

extern const U64 ROOK_MAGIC_NUMBERS[64];


extern U64 create_index_cover(int index, U64 attackMask, int bitAmount);

extern U64 calculate_bishop_attacks(Square square, U64 block);

extern U64 calculate_rook_attacks(Square square, U64 block);


U64 BISHOP_LOOKUP_ATTACKS[BOARD_SQUARES][512];

U64 ROOK_LOOKUP_ATTACKS[BOARD_SQUARES][4096];

void init_rook_lookup_attacks()
{
  for (Square square = 0; square < BOARD_SQUARES; square++)
  {
    int relevantBits = ROOK_RELEVANT_BITS[square];

    int coverIndicies = (1 << relevantBits);

    for (int index = 0; index < coverIndicies; index++)
    {
      U64 cover = create_index_cover(index, ROOK_LOOKUP_MASKS[square], relevantBits);

      int magicIndex = (cover * ROOK_MAGIC_NUMBERS[square]) >> (BOARD_SQUARES - relevantBits);

      ROOK_LOOKUP_ATTACKS[square][magicIndex] = calculate_rook_attacks(square, cover);
    }
  }
}

void init_bishop_lookup_attacks()
{
  for (Square square = 0; square < BOARD_SQUARES; square++)
  {
    int relevantBits = BISHOP_RELEVANT_BITS[square];

    int coverIndicies = (1 << relevantBits);

    for (int index = 0; index < coverIndicies; index++)
    {
      U64 cover = create_index_cover(index, BISHOP_LOOKUP_MASKS[square], relevantBits);

      int magicIndex = (cover * BISHOP_MAGIC_NUMBERS[square]) >> (BOARD_SQUARES - relevantBits);

      BISHOP_LOOKUP_ATTACKS[square][magicIndex] = calculate_bishop_attacks(square, cover);
    }
  }
}

void init_piece_lookup_attacks()
{
  init_rook_lookup_attacks();

  init_bishop_lookup_attacks();
}

int bishop_cover_index(Square square, U64 cover)
{
  U64 coverIndex = cover;

  coverIndex &= BISHOP_LOOKUP_MASKS[square];

  coverIndex *= BISHOP_MAGIC_NUMBERS[square];

  coverIndex >>= BOARD_SQUARES - BISHOP_RELEVANT_BITS[square];

  return (int) coverIndex;
}

int rook_cover_index(Square square, U64 cover)
{
  U64 coverIndex = cover;

  coverIndex &= ROOK_LOOKUP_MASKS[square];

  coverIndex *= ROOK_MAGIC_NUMBERS[square];

  coverIndex >>= BOARD_SQUARES - ROOK_RELEVANT_BITS[square];

  return (int) coverIndex;
}

U64 bishop_lookup_attacks(Square square, U64 cover)
{
  int coverIndex = bishop_cover_index(square, cover);

  return BISHOP_LOOKUP_ATTACKS[square][coverIndex];
}

U64 rook_lookup_attacks(Square square, U64 cover)
{
  int coverIndex = rook_cover_index(square, cover);

  return ROOK_LOOKUP_ATTACKS[square][coverIndex];
}

U64 queen_lookup_attacks(Square square, U64 cover)
{
  U64 queenAttacks = 0ULL;

  queenAttacks |= bishop_lookup_attacks(square, cover);

  queenAttacks |= rook_lookup_attacks(square, cover);

  return queenAttacks;
}

U64 pawn_lookup_attacks(Side side, Square square)
{
  return PAWN_LOOKUP_MASKS[side][square];
}

U64 knight_lookup_attacks(Square square)
{
  return KNIGHT_LOOKUP_MASKS[square];
}

U64 king_lookup_attacks(Square square)
{
  return KING_LOOKUP_MASKS[square];
}

Piece boards_square_piece(U64 boards[12], Square square)
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    if(BOARD_SQUARE_GET(boards[piece], square)) return piece;
  }
  return PIECE_NONE;
}

U64 piece_lookup_attacks(Position position, Square square)
{
  Piece piece = boards_square_piece(position.boards, square);

  if((piece == PIECE_WHITE_KING) || (piece == PIECE_BLACK_KING))
  {
    return king_lookup_attacks(square);
  }
  else if((piece == PIECE_WHITE_KNIGHT) || (piece == PIECE_BLACK_KNIGHT))
  {
    return knight_lookup_attacks(square);
  }
  else if((piece == PIECE_WHITE_BISHOP) || (piece == PIECE_BLACK_BISHOP))
  {
    return bishop_lookup_attacks(square, position.covers[SIDE_BOTH]);
  }
  else if((piece == PIECE_WHITE_ROOK) || (piece == PIECE_BLACK_ROOK))
  {
    return rook_lookup_attacks(square, position.covers[SIDE_BOTH]);
  }
  else if((piece == PIECE_WHITE_QUEEN) || (piece == PIECE_BLACK_QUEEN))
  {
    return queen_lookup_attacks(square, position.covers[SIDE_BOTH]);
  }
  else if(piece == PIECE_WHITE_PAWN)
  {
    return pawn_lookup_attacks(SIDE_WHITE, square);
  }
  else if(piece == PIECE_BLACK_PAWN)
  {
    return pawn_lookup_attacks(SIDE_BLACK, square);
  }
  else return 0ULL;
}
