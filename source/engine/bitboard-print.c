#include "../engine.h"

#include "engine-values.h"

void bitboard_print(U64 bitboard)
{
  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      printf("%c ", BITBOARD_SQUARE_GET(bitboard, square) ? 'X' : '.');
    }
    printf("\n");
  }
  printf("  A B C D E F G H\n");

  printf("\nBitboard: %llu\n\n", bitboard);
}

void position_print(Position position)
{
  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      int printPiece = -1;

      for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
      {
        if(BITBOARD_SQUARE_GET(position.bitboards[piece], square))
          printPiece = piece;
      }
      printf("%c ", (printPiece != -1) ? PIECE_SYMBOLS[printPiece] : '.');
    }
    printf("\n");
  }
  printf("  A B C D E F G H\n\n");

  printf("Side: %s\n", (position.side == SIDE_WHITE) ? "white" : "black");

  printf("Enpassant: %s\n", (position.enpassant != SQUARE_NONE) ? SQUARE_STRINGS[position.enpassant] : "no");

  printf("Castle: %c%c%c%c\n",
    (position.castle & CASTLE_WHITE_KING) ? 'K' : '-',
    (position.castle & CASTLE_WHITE_QUEEN) ? 'Q' : '-',
    (position.castle & CASTLE_BLACK_KING) ? 'k' : '-',
    (position.castle & CASTLE_BLACK_QUEEN) ? 'q' : '-'
  );
}