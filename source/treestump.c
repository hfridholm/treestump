#include "engine.h"

#include "engine/piece-lookup-attacks.h"

U64 BOARD_LOOKUP_LINES[BOARD_SQUARES][BOARD_SQUARES];

U64 create_board_line(Square source, Square target)
{
  U64 board = 0ULL;

  int sourceRank = (source / BOARD_FILES);
  int sourceFile = (source % BOARD_FILES);

  int targetRank = (target / BOARD_FILES);
  int targetFile = (target % BOARD_FILES);

  int rankOffset = (targetRank - sourceRank);
  int fileOffset = (targetFile - sourceFile);

  int rankFactor = (rankOffset > 0) ? +1 : -1;
  int fileFactor = (fileOffset > 0) ? +1 : -1;

  int absRankOffset = (rankOffset * rankFactor);
  int absFileOffset = (fileOffset * fileFactor);

  // If the move is not diagonal nor straight, return empty board;
  if(!(absRankOffset == absFileOffset) && !((absRankOffset == 0) ^ (absFileOffset == 0))) return 0ULL;

  int rankScalor = (rankOffset == 0) ? 0 : rankFactor;
  int fileScalor = (fileOffset == 0) ? 0 : fileFactor;

  for(int rank = sourceRank, file = sourceFile; (rank != targetRank || file != targetFile); rank += rankScalor, file += fileScalor)
  {
    Square square = (rank * BOARD_FILES) + file;

    if(square == source || square == target) continue;

    board = BOARD_SQUARE_SET(board, square);
  }
  return board;
}

void init_board_lookup_lines()
{
  for(Square sourceSquare = A8; sourceSquare <= H1; sourceSquare++)
  {
    for(Square targetSquare = A8; targetSquare <= H1; targetSquare++)
    {
      U64 boardLines = create_board_line(sourceSquare, targetSquare);

      BOARD_LOOKUP_LINES[sourceSquare][targetSquare] = boardLines;
    }
  }
}

bool stdin_string(char* string, const char prompt[])
{
  fflush(stdin);
  printf("%s", prompt);
  char buffer[1024];
  if(fgets(buffer, sizeof(buffer), stdin) == NULL) return false;
  return sscanf(buffer, "%[^\n]%*c", string);
}

Square parse_square(const char squareString[])
{
  int file = squareString[0] - 'a';
  int rank = BOARD_RANKS - (squareString[1] - '0');

  if(!(file >= 0 && file < BOARD_FILES) || !(rank >= 0 && rank < BOARD_RANKS)) return SQUARE_NONE;

  return (rank * BOARD_FILES) + file;
}

Move parse_move(const char moveString[])
{
  Move parseMove = 0;

  Square sourceSquare = parse_square(moveString += 0);
  if(sourceSquare == SQUARE_NONE) return 0;

  Square targetSquare = parse_square(moveString += 2);
  if(targetSquare == SQUARE_NONE) return 0;

  parseMove |= MOVE_SET_SOURCE(sourceSquare);
  parseMove |= MOVE_SET_TARGET(targetSquare);

  Piece promotePiece = SYMBOL_PIECES[(unsigned char) *moveString];

  if(promotePiece != PIECE_WHITE_PAWN) parseMove |= MOVE_SET_PROMOTE(promotePiece);

  return parseMove;
}

char* move_string(char* moveString, Move move)
{
  const char* sourceString = SQUARE_STRINGS[MOVE_GET_SOURCE(move)];
  const char* targetString = SQUARE_STRINGS[MOVE_GET_TARGET(move)];

  Piece promotePiece = MOVE_GET_PROMOTE(move);

  if(promotePiece == PIECE_WHITE_PAWN) sprintf(moveString, "%s%s", sourceString, targetString);

  else sprintf(moveString, "%s%s%c", sourceString, targetString, PIECE_SYMBOLS[promotePiece]);

  return moveString;
}

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

const char FEN_START[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

Position parse_position(const char positionString[])
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
      Move parsedMove = parse_move(movesString);

      if(parsedMove == 0) break;

      Move move = complete_move(position.boards, parsedMove);

      make_move(&position, move);

      while(*movesString && *movesString != ' ') movesString++;

      movesString++;
    }
  }
  return position;
}

void init_all()
{
  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();

  init_board_lookup_lines();
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

void parse_go(Position* position, const char goString[])
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
    int depth = atoi(goString + 5);

    searchedNodes = 0;

    Move bestMove = best_move(*position, depth);

    char moveString[8];
    move_string(moveString, bestMove);

    printf("bestmove: %s\n", moveString);

    printf("searched nodes: %llu\n", searchedNodes);
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

int main(int argc, char* argv[])
{
  init_all();

  char inputString[256];

  /*stdin_string(inputString, "");

  if(!strcmp(inputString, "uci"))
  {
    printf("id name TreeStump\n");
    printf("id author Hampus Fridholm\n");
    printf("uciok\n");
  }
  else return 1;*/

  Position position;
  parse_fen(&position, FEN_START);

  while(strcmp(inputString, "quit"))
  {
    memset(inputString, 0, sizeof(inputString));

    stdin_string(inputString, "");

    if(!strncmp(inputString, "position", 8))
    {
      position = parse_position(inputString + 9);
    }
    else if(!strncmp(inputString, "go", 2))
    {
      parse_go(&position, inputString + 3);
    }
    else if(!strcmp(inputString, "d"))
    {
      position_print(position);
    }
    else if(!strncmp(inputString, "isready", 7))
    {
      printf("readyok\n");
    }
    else if(!strncmp(inputString, "debug", 5))
    {

    }
    else if(!strcmp(inputString, "ucinewgame"))
    {

    }
    else if(!strcmp(inputString, "stop"))
    {
      printf("bestmove a1a1\n");
    }
    else if(!strcmp(inputString, "ponderhit"))
    {

    }
  }

  return 0;
}