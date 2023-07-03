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

bool input_stdin_string(char* string, const char prompt[])
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
    printf("fen: (%s)\n", positionString + 4);
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

int main(int argc, char* argv[])
{
  printf("\n[ treestump ]\n\n");

  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();


  init_board_lookup_lines();


  Position position;
  parse_fen(&position, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  char inputString[256];

  while(strcmp(inputString, "quit"))
  {
    memset(inputString, 0, sizeof(inputString));

    input_stdin_string(inputString, "");

    if(!strncmp(inputString, "position", 8))
    {
      char* positionString = inputString + 9;

      position = parse_position(positionString);
    }
    else if(!strncmp(inputString, "go", 2))
    {
      char* goString = inputString + 3;

      if(!strncmp(goString, "perft", 5))
      {
        char* perftString = goString + 6;

        int depth = atoi(perftString);

        perft_test(position, depth);
      }
    }
    else if(!strncmp(inputString, "d", 1))
    {
      position_print(position);
    }
  }

  // parse_fen(&position, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  // parse_fen(&position, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

  // parse_fen(&position, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

  // parse_fen(&position, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

  /*
  for(int rank = 0; rank < 8; rank++)
  {
    for(int file = 0; file < 8; file++)
    {
      Square square = (rank * 8) + file;

      if(file == 0) printf("%d ", 8 - rank);

      if(board_square_attacked(position, square, !position.side)) printf("X ");
      else printf(". ");
    }
    printf("\n");
  }
  */

  /*
  MoveArray moveArray;

  memset(moveArray.moves, 0, sizeof(moveArray.moves));
  moveArray.amount = 0;

  create_moves(&moveArray, position);


  position_print(position);


  for(int index = 0; index < moveArray.amount; index++)
  {
    Square sourceSquare = MOVE_GET_SOURCE(moveArray.moves[index]);
    Square targetSquare = MOVE_GET_TARGET(moveArray.moves[index]);

    Piece promotePiece = MOVE_GET_PROMOTE(moveArray.moves[index]);

    printf("#%d %s%s%c\n", index + 1,
      SQUARE_STRINGS[sourceSquare], 
      SQUARE_STRINGS[targetSquare], 
      (promotePiece != PIECE_WHITE_PAWN) ? PIECE_SYMBOLS[promotePiece] : ' '
    );
  }
  */

  /*
  char inputMove[64];

  while(true)
  {
    board_covers_print(position);


    if(!input_stdin_string(inputMove, "Move: ")) break;

    Move parseMove = 0;

    if(!parse_string_move(&parseMove, inputMove))
    {
      printf("parse failed\n");
    }

    parseMove = complete_board_move(position.boards, parseMove);

    
    printf("Move: (%s)\n", inputMove);

    printf("Source  : %s\n", SQUARE_STRINGS[MOVE_GET_SOURCE(parseMove)]);
    printf("Target  : %s\n", SQUARE_STRINGS[MOVE_GET_TARGET(parseMove)]);
    printf("Piece   : %c\n", PIECE_SYMBOLS[MOVE_GET_PIECE(parseMove)]);
    printf("Promote : %c\n", (parseMove & MOVE_MASK_PROMOTE) ? PIECE_SYMBOLS[MOVE_GET_PROMOTE(parseMove)] : '-');

    printf("Capture : %s\n", (parseMove & MOVE_MASK_CAPTURE) ? "yes" : "no");
    printf("Double  : %s\n", (parseMove & MOVE_MASK_DOUBLE) ? "yes" : "no");
    printf("Passant : %s\n", (parseMove & MOVE_MASK_PASSANT) ? "yes" : "no");
    printf("Castle  : %s\n", (parseMove & MOVE_MASK_CASTLE) ? "yes" : "no");


    
    bool legal = move_fully_legal(position, parseMove);

    printf("\nLegal? %s\n", legal ? "yes" : "no");

    if(legal) make_move(&position, parseMove);
  }
  */

  return 0;
}