#include "../treestump.h"

extern int board_ls1b_index(U64 bitboard);


const int SQUARE_SCORES_PAWN[BOARD_SQUARES] = 
{
  90, 90, 90, 90, 90, 90, 90, 90,
  30, 30, 30, 40, 40, 30, 30, 30,
  20, 20, 20, 30, 30, 30, 20, 20,
  10, 10, 10, 20, 20, 10, 10, 10,
   5,  5, 10, 20, 20,  5,  5,  5,
   0,  0,  0,  5,  5,  0,  0,  0,
   0,  0,  0,-10,-10,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0
};

const int SQUARE_SCORES_KNIGHT[BOARD_SQUARES] = 
{
  -5,  0,  0,  0,  0,  0,  0, -5,
  -5,  0,  0, 10, 10,  0,  0, -5,
  -5,  5, 20, 20, 20, 20,  5, -5,
  -5, 10, 20, 30, 30, 20, 10, -5,
  -5, 10, 20, 30, 30, 20, 10, -5,
  -5,  5, 20, 10, 10, 20,  5, -5,
  -5,  0,  0,  0,  0,  0,  0, -5,
  -5,-10,  0,  0,  0,  0,-10, -5
};

const int SQUARE_SCORES_BISHOP[BOARD_SQUARES] = 
{
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 10, 10,  0,  0,  0,
  0,  0, 10, 20, 20, 10,  0,  0,
  0,  0, 10, 20, 20, 10,  0,  0,
  0, 10,  0,  0,  0,  0, 10,  0,
  0, 30,  0,  0,  0,  0, 30,  0,
  0,  0,-10,  0,  0,-10,  0,  0
};

const int SQUARE_SCORES_ROOK[BOARD_SQUARES] = 
{
  50, 50, 50, 50, 50, 50, 50, 50,
  50, 50, 50, 50, 50, 50, 50, 50,
   0,  0, 10, 20, 20, 10,  0,  0,
   0,  0, 10, 20, 20, 10,  0,  0,
   0,  0, 10, 20, 20, 10,  0,  0,
   0,  0, 10, 20, 20, 10,  0,  0,
   0,  0, 10, 20, 20, 10,  0,  0,
   0,  0,  0, 20, 20,  0,  0,  0
};

const int SQUARE_SCORES_KING[BOARD_SQUARES] = 
{
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  5,  5,  5,  5,  0,  0,
  0,  5,  5, 10, 10,  5,  5,  0,
  0,  5, 10, 20, 20, 10,  5,  0,
  0,  5, 10, 20, 20, 10,  5,  0,
  0,  0,  5, 10, 10,  5,  0,  0,
  0,  5,  5, -5, -5,  0,  5,  0,
  0,  0,  5,  0,-15,  0, 10,  0
};

const Square MIRROR_SQUARES[BOARD_SQUARES] =
{
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8
};

const int PIECE_SCORES[12] = {100, 300, 350, 500, 1000, 10000, -100, -300, -350, -500, -1000, -10000};

int piece_square_score(Piece piece, Square square)
{
  switch(piece)
  {
    case PIECE_WHITE_PAWN:    return SQUARE_SCORES_PAWN[square];
    case PIECE_WHITE_KNIGHT:  return SQUARE_SCORES_KNIGHT[square];
    case PIECE_WHITE_BISHOP:  return SQUARE_SCORES_BISHOP[square];
    case PIECE_WHITE_ROOK:    return SQUARE_SCORES_ROOK[square];
    //case PIECE_WHITE_QUEEN:   return SQUARE_SCORES_QUEEN[square];
    case PIECE_WHITE_KING:    return SQUARE_SCORES_KING[square];

    case PIECE_BLACK_PAWN:    return -SQUARE_SCORES_PAWN[MIRROR_SQUARES[square]];
    case PIECE_BLACK_KNIGHT:  return -SQUARE_SCORES_KNIGHT[MIRROR_SQUARES[square]];
    case PIECE_BLACK_BISHOP:  return -SQUARE_SCORES_BISHOP[MIRROR_SQUARES[square]];
    case PIECE_BLACK_ROOK:    return -SQUARE_SCORES_ROOK[MIRROR_SQUARES[square]];
    //case PIECE_BLACK_QUEEN:   return -SQUARE_SCORES_QUEEN[MIRROR_SQUARES[square]];
    case PIECE_BLACK_KING:    return -SQUARE_SCORES_KING[MIRROR_SQUARES[square]];

    default: return 0;
  }
}

int position_board_score(U64 boards[12])
{
  int boardScore = 0;

  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    U64 bitboard = boards[piece];

    while(bitboard)
    {
      Square square = board_ls1b_index(bitboard);


      boardScore += PIECE_SCORES[piece];
      boardScore += piece_square_score(piece, square);



      bitboard = BOARD_SQUARE_POP(bitboard, square);
    }
  }
  return boardScore;
}

int position_score(Position position)
{
  int positionScore = 0;

  positionScore += position_board_score(position.boards);
  
  return positionScore;
}