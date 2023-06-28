#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned long long  U64;
typedef unsigned long       U32;

#define BITBOARD_SQUARE_SET(BITBOARD, SQUARE) ((BITBOARD) |  (1ULL << (SQUARE)))
#define BITBOARD_SQUARE_GET(BITBOARD, SQUARE) ((BITBOARD) &  (1ULL << (SQUARE)))
#define BITBOARD_SQUARE_POP(BITBOARD, SQUARE) ((BITBOARD) & ~(1ULL << (SQUARE)))

#define BOARD_FILES   8
#define BOARD_RANKS   8
#define BOARD_SQUARES 64

typedef enum
{
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1,
  SQUARE_NONE
} Square;

typedef enum 
{
  SIDE_WHITE, 
  SIDE_BLACK, 
  SIDE_BOTH 
} Side;

typedef unsigned char Castle;

typedef enum
{
  PIECE_WHITE_PAWN, 
  PIECE_WHITE_KNIGHT, 
  PIECE_WHITE_BISHOP, 
  PIECE_WHITE_ROOK, 
  PIECE_WHITE_QUEEN, 
  PIECE_WHITE_KING,
  PIECE_BLACK_PAWN, 
  PIECE_BLACK_KNIGHT, 
  PIECE_BLACK_BISHOP, 
  PIECE_BLACK_ROOK, 
  PIECE_BLACK_QUEEN, 
  PIECE_BLACK_KING
} Piece;

typedef struct
{
  U64 bitboards[12];
  U64 occupancies[3];
  Side side;
  Square enpassant;
  Castle castle;
} Position;

extern void bitboard_print(U64 bitboard);

#endif // ENGINE_H