#include "../engine.h"

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

  printf("\nBitboard: %llu\n", bitboard);
}