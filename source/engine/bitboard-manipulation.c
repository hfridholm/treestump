#include "../engine.h"

int bitboard_bit_count(U64 bitboard)
{
  int count = 0;

  // for(count = 0; bitboard; ++count)
  while(bitboard)
  {
    count++;
    bitboard &= bitboard - 1;
  }

  return count;
}

int bitboard_ls1b_index(U64 bitboard)
{
  if(!bitboard) return -1;

  return bitboard_bit_count((bitboard & -bitboard) - 1);
}