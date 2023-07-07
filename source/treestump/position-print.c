#include "../treestump.h"

extern const char PIECE_SYMBOLS[12];

extern const char* SQUARE_STRINGS[BOARD_SQUARES];

extern const Castle CASTLE_BLACK_QUEEN;
extern const Castle CASTLE_BLACK_KING;
extern const Castle CASTLE_WHITE_QUEEN;
extern const Castle CASTLE_WHITE_KING;

extern Piece boards_square_piece(U64 boards[12], Square square);


void bitboard_print(U64 bitboard)
{
  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      printf("%c ", BOARD_SQUARE_GET(bitboard, square) ? 'X' : '.');
    }
    printf("\n");
  }
  printf("  A B C D E F G H\n");

  printf("\nBitboard: %llu\n\n", bitboard);
}

void position_info_print(Position position)
{
  printf("Side:      %s\n", (position.side == SIDE_WHITE) ? "white" : "black");

  printf("Enpassant: %s\n", (position.passant != SQUARE_NONE) ? SQUARE_STRINGS[position.passant] : "no");

  printf("Castling:  %c%c%c%c\n",
    (position.castle & CASTLE_WHITE_KING) ? 'K' : '-',
    (position.castle & CASTLE_WHITE_QUEEN) ? 'Q' : '-',
    (position.castle & CASTLE_BLACK_KING) ? 'k' : '-',
    (position.castle & CASTLE_BLACK_QUEEN) ? 'q' : '-'
  );
}

void position_print(Position position)
{
  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      int piece = boards_square_piece(position.boards, square);

      printf("%c ", (piece != PIECE_NONE) ? PIECE_SYMBOLS[piece] : '.');
    }
    printf("\n");
  }
  printf("  A B C D E F G H\n\n");

  position_info_print(position);
}

void board_covers_print(Position position)
{
  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      int piece = boards_square_piece(position.boards, square);

      printf("%c ", (piece != PIECE_NONE) ? PIECE_SYMBOLS[piece] : '.');
    }

    printf("  |  ");

    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      printf("%c ", BOARD_SQUARE_GET(position.covers[SIDE_BOTH], square) ? 'X' : '.');
    }

    printf("  |  ");

    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      printf("%c ", BOARD_SQUARE_GET(position.covers[SIDE_WHITE], square) ? 'X' : '.');
    }

    printf("  |  ");

    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      if(!file) printf("%d ", BOARD_RANKS - rank);

      printf("%c ", BOARD_SQUARE_GET(position.covers[SIDE_BLACK], square) ? 'X' : '.');
    }

    printf("\n");
  }
  printf("  A B C D E F G H\n\n");

  position_info_print(position);
}