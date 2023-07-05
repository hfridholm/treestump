#include "../treestump.h"

const char FEN_START[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

extern U64 searchedNodes;

extern bool parse_fen(Position* position, const char fenString[]);

extern void perft_test(Position position, int depth);

extern Move best_move(Position position, int depth);

extern char* move_string(char* moveString, Move move);

extern Move string_move(const char moveString[]);

extern Move complete_move(U64 boards[12], Move move);

extern void position_print(Position position);

extern void make_move(Position* position, Move move);


void parse_uci_go_depth(Position position, const char depthString[])
{
  int depth = atoi(depthString);

  searchedNodes = 0;

  Move bestMove = best_move(position, depth);

  char moveString[8];
  move_string(moveString, bestMove);

  printf("info searched nodes: %llu\n", searchedNodes);

  printf("bestmove %s\n", moveString);
}

void parse_uci_go(Position* position, const char goString[])
{
  if(!strncmp(goString, "perft", 5))
  {
    int depth = atoi(goString + 6);

    perft_test(*position, depth);
  }
  else if(!strncmp(goString, "searchmoves", 11))
  {

  }
  else if(!strncmp(goString, "ponder", 5))
  {

  }
  else if(!strncmp(goString, "wtime", 5))
  {

  }
  else if(!strncmp(goString, "btime", 5))
  {

  }
  else if(!strncmp(goString, "winc", 4))
  {

  }
  else if(!strncmp(goString, "binc", 5))
  {

  }
  else if(!strncmp(goString, "depth", 5))
  {
    parse_uci_go_depth(*position, goString + 5);
  } 
  else if(!strncmp(goString, "nodes", 5))
  {

  }
  else if(!strncmp(goString, "mate", 4))
  {

  }
  else if(!strncmp(goString, "movetime", 8))
  {

  }
  else if(!strncmp(goString, "infinite", 8))
  {

  }
}

Position parse_uci_position(const char positionString[])
{
  Position position;

  if(!strncmp(positionString, "startpos", 8))
  {
    parse_fen(&position, FEN_START);
  }
  else if(!strncmp(positionString, "fen", 3))
  {
    parse_fen(&position, positionString + 4);
  }
  else parse_fen(&position, FEN_START);


  char* movesString = strstr(positionString, "moves");

  if(movesString != NULL)
  {
    movesString += 6;

    while(*movesString)
    {
      Move parsedMove = string_move(movesString);

      if(parsedMove == 0) break;

      Move move = complete_move(position.boards, parsedMove);

      make_move(&position, move);

      while(*movesString && *movesString != ' ') movesString++;

      movesString++;
    }
  }
  return position;
}

void parse_uci(Position* position, const char uciString[])
{
  if(!strncmp(uciString, "position", 8))
  {
    *position = parse_uci_position(uciString + 9);
  }
  else if(!strncmp(uciString, "go", 2))
  {
    parse_uci_go(position, uciString + 3);
  }
  else if(!strcmp(uciString, "d"))
  {
    position_print(*position);
  }
  else if(!strncmp(uciString, "isready", 7))
  {
    printf("readyok\n");
  }
  else if(!strncmp(uciString, "debug", 5))
  {

  }
  else if(!strcmp(uciString, "ucinewgame"))
  {

  }
  else if(!strcmp(uciString, "stop"))
  {
    printf("bestmove a1a1\n");
  }
  else if(!strcmp(uciString, "ponderhit"))
  {

  }
}