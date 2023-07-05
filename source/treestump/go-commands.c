#include "../treestump.h"

extern char* move_string(char* moveString, Move move);

extern void create_moves(MoveArray* moveArray, Position position);

extern void make_move(Position* position, Move move);

extern int board_ls1b_index(U64 bitboard);

extern int position_score(Position position);

extern bool board_square_attacked(Position position, Square square, Side side);

extern void guess_order_moves(MoveArray* moveArray, Position position);


U64 perft_driver(Position position, int depth)
{
  if(depth <= 0) return 1;

  MoveArray moveArray;

  memset(moveArray.moves, 0, sizeof(moveArray.moves));
  moveArray.amount = 0;

  create_moves(&moveArray, position);

  int nodes = 0;

  for(int index = 0; index < moveArray.amount; index++)
  {
    Position positionCopy = position;

    make_move(&positionCopy, moveArray.moves[index]);

    nodes += perft_driver(positionCopy, depth - 1);
  }
  return nodes;
}

void perft_test(Position position, int depth)
{
  MoveArray moveArray;

  memset(moveArray.moves, 0, sizeof(moveArray.moves));
  moveArray.amount = 0;

  create_moves(&moveArray, position);

  char moveString[8];

  U64 totalNodes = 0;

  for(int index = 0; index < moveArray.amount; index++)
  {
    Position positionCopy = position;

    make_move(&positionCopy, moveArray.moves[index]);

    U64 moveNodes = perft_driver(positionCopy, depth - 1);

    totalNodes += moveNodes;

    memset(moveString, 0, sizeof(moveString));

    move_string(moveString, moveArray.moves[index]);

    printf("%s: %llu\n", moveString, moveNodes);
  }
  printf("\nNodes searched: %llu\n", totalNodes);
}

U64 searchedNodes = 0;

int negamax(Position position, int depth, int alpha, int beta)
{
  if(depth <= 0)
  {
    searchedNodes++;

    int score = position_score(position);

    return (position.side == SIDE_WHITE) ? score : -score;
  }

  int bestScore = -500000;

  MoveArray moveArray;

  memset(moveArray.moves, 0, sizeof(moveArray.moves));
  moveArray.amount = 0;

  create_moves(&moveArray, position);


  if(moveArray.amount <= 0)
  {
    U64 kingBoard = (position.side == SIDE_WHITE) ? position.boards[PIECE_WHITE_KING] : position.boards[PIECE_BLACK_KING];

    Square kingSquare = board_ls1b_index(kingBoard);

    if(kingSquare == -1 || board_square_attacked(position, kingSquare, !position.side))
    {
      return -49000 + depth;
    }
    else return 0; // Draw;
  }


  guess_order_moves(&moveArray, position);

  for(int index = 0; index < moveArray.amount; index++)
  {
    Position positionCopy = position;

    make_move(&positionCopy, moveArray.moves[index]);

    int currentScore = -negamax(positionCopy, (depth - 1), -beta, -alpha);

    if(currentScore > bestScore) bestScore = currentScore;

    if(bestScore > alpha) alpha = bestScore;

    if(alpha >= beta) break;
  }
  return bestScore;
}

Move best_move(Position position, int depth)
{
  MoveArray moveArray;

  memset(moveArray.moves, 0, sizeof(moveArray.moves));
  moveArray.amount = 0;

  create_moves(&moveArray, position);

  if(moveArray.amount <= 0) return 0;

  int bestScore = -500000;
  Move bestMove = moveArray.moves[0];

  for(int index = 0; index < moveArray.amount; index++)
  {
    Position positionCopy = position;

    Move currentMove = moveArray.moves[index];

    make_move(&positionCopy, currentMove);

    int currentScore = -negamax(positionCopy, (depth - 1), -50000, +50000);

    /*
    char moveString[8];
    move_string(moveString, currentMove);
    printf("%s: %d\n", moveString, currentScore);
    */

    if(currentScore > bestScore) 
    {
      bestScore = currentScore;
      bestMove = currentMove;
    }
  }
  return bestMove;
}