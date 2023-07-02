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

bool parse_string_square(Square* square, const char string[])
{
  if(strlen(string) != 2) return false;

  int file = string[0] - 'a';
  int rank = BOARD_RANKS - (string[1] - '0');

  if(!(file >= 0 && file < BOARD_FILES) || !(rank >= 0 && rank < BOARD_RANKS)) return false;

  *square = (rank * BOARD_FILES) + file;

  return true;
}

bool parse_string_move(Move* move, const char stringMove[])
{
  Move parseMove = 0;

  int stringLength = strlen(stringMove);

  if(stringLength != 4 && stringLength != 5) return false;

  char sourceString[3];
  memset(sourceString, '\0', sizeof(sourceString));
  strncpy(sourceString, (stringMove + 0), 2);

  Square sourceSquare;
  if(!parse_string_square(&sourceSquare, sourceString)) return false;

  char targetString[3];
  memset(targetString, '\0', sizeof(targetString));
  strncpy(targetString, (stringMove + 2), 2);

  Square targetSquare;
  if(!parse_string_square(&targetSquare, targetString)) return false;

  /*
  if(stringLength == 5)
  {
    switch(stringMove[4])
    {
    case 'q': sourceString = SET_MOVE_PROMOTE(());
    }
  }
  */

  parseMove |= MOVE_SET_SOURCE(sourceSquare);
  parseMove |= MOVE_SET_TARGET(targetSquare);

  *move = parseMove;

  return true;
}

int captureCount = 0;
int passantCount = 0;
int castleCount = 0;
int promoteCount = 0;

unsigned long position_depth_nodes(Position position, int depth)
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


    if(moveArray.moves[index] & MOVE_MASK_CAPTURE) captureCount++;
    if(moveArray.moves[index] & MOVE_MASK_PASSANT) passantCount++;
    if(moveArray.moves[index] & MOVE_MASK_CASTLE)  castleCount++;    
    if(MOVE_GET_PROMOTE(moveArray.moves[index]) != PIECE_WHITE_PAWN) promoteCount++;


    nodes += position_depth_nodes(positionCopy, depth - 1);
  }
  return nodes;
}

int main(int argc, char* argv[])
{
  printf("\n[ treestump ]\n\n");

  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();


  init_board_lookup_lines();



  Position position;

  // nodes=over captures=over passants=exact 
  parse_fen(&position, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  // nodes=over captures=over passants=exact 
  // parse_fen(&position, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

  // nodes=over captures=over passants=over promotions=failed
  // parse_fen(&position, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

  // nodes=under captures=under passants=under promotions=failed
  // parse_fen(&position, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

  int depth = 5;

  unsigned long nodes = position_depth_nodes(position, depth);

  printf("depth: %d\tnodes: %lu\tcapture: %d\tpassant: %d\tcastles: %d\tpromotes: %d\n", 
    depth, nodes, captureCount, passantCount, castleCount, promoteCount);

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