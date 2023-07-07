#include "treestump.h"

void init_all(void)
{
  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();

  init_board_lookup_lines();

  init_random_keys();
}

bool stdin_string(char* string)
{
  fflush(stdin);

  char buffer[1024];

  if(fgets(buffer, sizeof(buffer), stdin) == NULL) return false;

  return (sscanf(buffer, "%[^\n]%*c", string) == 1);
}

extern const char FEN_START[];

extern bool parse_fen(Position* position, const char fenString[]);

int main(int argc, char* argv[])
{
  init_all();

  char uciString[256];
  memset(uciString, 0, sizeof(uciString));

  stdin_string(uciString);

  if(!strcmp(uciString, "uci"))
  {
    printf("id name TreeStump\n");
    printf("id author Hampus Fridholm\n");
    printf("uciok\n");
  }
  else return 1;

  Position position;
  parse_fen(&position, FEN_START);

  do
  {
    memset(uciString, 0, sizeof(uciString));

    stdin_string(uciString);

    parse_uci(&position, uciString);
  }
  while(strcmp(uciString, "quit"));

  return 0;
}