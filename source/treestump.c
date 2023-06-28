#include "engine.h"

int main(int argc, char* argv[])
{
  printf("\n[ treestump ]\n\n");

  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();


  const char fenString[] = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";


  Position position;

  if(!parse_fen_string(&position, fenString))
  {
    printf("parse_fen_string failed!\n");
  }
  else printf("parse_fen_string succesful!\n");
  

  position_print(position);

  return 0;
}