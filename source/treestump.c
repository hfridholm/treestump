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

bool board_square_attacked(Position position, Square square, Side side)
{
  if(queen_lookup_attacks(square, position.covers[SIDE_BOTH])   & ((side == SIDE_WHITE) ? position.boards[PIECE_WHITE_QUEEN]  : position.boards[PIECE_BLACK_QUEEN]))  return true;

  if(bishop_lookup_attacks(square, position.covers[SIDE_BOTH])  & ((side == SIDE_WHITE) ? position.boards[PIECE_WHITE_BISHOP] : position.boards[PIECE_BLACK_BISHOP])) return true;

  if(rook_lookup_attacks(square, position.covers[SIDE_BOTH])    & ((side == SIDE_WHITE) ? position.boards[PIECE_WHITE_ROOK]   : position.boards[PIECE_BLACK_ROOK]))   return true;


  if(side == SIDE_WHITE && (pawn_lookup_attacks(SIDE_BLACK, square) & position.boards[PIECE_WHITE_PAWN]))                                                             return true;

  if(side == SIDE_BLACK && (pawn_lookup_attacks(SIDE_BLACK, square) & position.boards[PIECE_BLACK_PAWN]))                                                             return true;

  if(knight_lookup_attacks(square)                              & ((side == SIDE_WHITE) ? position.boards[PIECE_WHITE_KNIGHT] : position.boards[PIECE_BLACK_KNIGHT])) return true;
  

  if(king_lookup_attacks(square)                                & ((side == SIDE_WHITE) ? position.boards[PIECE_WHITE_KING]   : position.boards[PIECE_BLACK_KING]))   return true;

  return false;
}

bool validate_double_move()
{
  return false;
}

bool validate_passant_move()
{
  return false;
}

bool validate_castle_move()
{
  return false;
}

bool move_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);


  // If the moving piece does not exists on the source square
  if(!BOARD_SQUARE_GET(position.boards[sourcePiece], sourceSquare)) return false;



  Piece targetPiece = boards_square_piece(position.boards, targetSquare);

  // If the capture flag and the target piece doesn't match each other
  if((move & MOVE_MASK_CAPTURE) ^ (targetPiece != PIECE_NONE)) return false;


  bool sourceWhite = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING);
  bool targetWhite = (targetPiece >= PIECE_BLACK_PAWN && sourcePiece <= PIECE_BLACK_KING);

  // If source and target are not of different sides (same sides)
  if((move & MOVE_MASK_CAPTURE) && !(sourceWhite ^ targetWhite)) return false;


  // If there are any pieces between source and target square
  if(BOARD_LOOKUP_LINES[sourceSquare][targetSquare] & position.covers[SIDE_BOTH]) return false;



  if((sourcePiece == PIECE_WHITE_PAWN) || (sourcePiece == PIECE_BLACK_PAWN))
  {
    if(move & MOVE_MASK_CAPTURE)
    {
      if(sourcePiece == PIECE_WHITE_PAWN)
      {
        if(!(pawn_lookup_attacks(SIDE_WHITE, sourceSquare) & BOARD_SQUARE_SET(0ULL, targetSquare))) return false;
      }
      else if(sourcePiece == PIECE_BLACK_PAWN)
      {
        if(!(pawn_lookup_attacks(SIDE_BLACK, sourceSquare) & BOARD_SQUARE_SET(0ULL, targetSquare))) return false;
      }
      else return false;


      if(move & MOVE_MASK_PASSANT)
      {

      }
    }
    else if(move & MOVE_MASK_DOUBLE)
    {
      if(sourcePiece == PIECE_WHITE_PAWN)
      {
        if(!((sourceSquare >= A2) && (sourceSquare <= H2))) return false;

        if((sourceSquare - targetSquare) != (BOARD_FILES * 2)) return false;
      }
      else if(sourcePiece == PIECE_BLACK_PAWN)
      {
        if(!((sourceSquare >= A7) && (sourceSquare <= H7))) return false;

        if((targetSquare - sourceSquare) != (BOARD_FILES * 2)) return false;
      }
      else return false;
    }
    else // Single move forward
    {
      if(sourcePiece == PIECE_WHITE_PAWN)
      {
        if((sourceSquare - targetSquare) != BOARD_FILES) return false;
      }
      else if(sourcePiece == PIECE_BLACK_PAWN)
      {
        if((targetSquare - sourceSquare) != BOARD_FILES) return false;
      }
      else return false;
    }

    if(move & MOVE_MASK_PROMOTE)
    {
      if(sourcePiece == PIECE_WHITE_PAWN)
      {
        if(!(targetSquare >= A8 && targetSquare <= H8)) return false;
      }
      else if(sourcePiece == PIECE_BLACK_PAWN)
      {
        if(!(targetSquare >= A1 && targetSquare <= H1)) return false;
      }
      else return false;
    }
  }
  else if((sourcePiece == PIECE_WHITE_KING) && (move & MOVE_MASK_CASTLE))
  {
    if(sourceSquare != E1) return false;

    if(targetSquare == G1)
    {
      if(!BOARD_SQUARE_GET(position.boards[PIECE_WHITE_ROOK], H1)) return false;
    }
    else if(targetSquare == C1)
    {
      if(!BOARD_SQUARE_GET(position.boards[PIECE_WHITE_ROOK], A1)) return false;
    }
    else return false;
  }
  else if((sourcePiece == PIECE_BLACK_KING) && (move & MOVE_MASK_CASTLE))
  {
    if(sourceSquare != E8) return false;

    if(targetSquare == G8)
    {
      if(!BOARD_SQUARE_GET(position.boards[PIECE_WHITE_ROOK], H8)) return false;
    }
    else if(targetSquare == C8)
    {
      if(!BOARD_SQUARE_GET(position.boards[PIECE_WHITE_ROOK], A8)) return false;
    }
    else return false;
  }
  else // knights, bishops, rooks, queens, and kings that do not castle
  {
    if(!(piece_lookup_attacks(position, sourceSquare) & BOARD_SQUARE_SET(0ULL, targetSquare))) return false;
  }

  return true;
}

bool move_fully_legal(Position position, Move move)
{
  if(!move_pseudo_legal(position, move)) return false;

  return true;
}

void make_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);


  position->boards[sourcePiece] = BOARD_SQUARE_POP(position->boards[sourcePiece], sourceSquare);


  if(move & MOVE_MASK_CAPTURE || sourcePiece == PIECE_WHITE_PAWN || sourcePiece == PIECE_BLACK_PAWN)
  {
    position->clock = 0;
  }
  else position->clock++;


  if(sourcePiece >= PIECE_BLACK_PAWN && sourcePiece <= PIECE_BLACK_KING)
  {
    position->turns++;
  }


  position->side = (position->side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;


  if(move & MOVE_MASK_CAPTURE) // Capture
  {
    Piece targetPiece = boards_square_piece(position->boards, targetSquare);

    if(targetPiece != PIECE_NONE)
    {
      position->boards[targetPiece] = BOARD_SQUARE_POP(position->boards[targetPiece], targetSquare);
    }
  }

  if(sourcePiece == PIECE_WHITE_KING) position->castle &= ~(CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN);

  else if(sourcePiece == PIECE_WHITE_KING) position->castle &= ~(CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN);


  if(move & MOVE_MASK_CASTLE) // Castle
  {
    Piece rookPiece = (sourcePiece == PIECE_WHITE_KING) ? PIECE_WHITE_ROOK : PIECE_BLACK_ROOK;

    Square rookSourceSquare = (targetSquare > sourceSquare) ? (sourceSquare + 3) : (sourceSquare - 4);

    Square rookTargetSquare = (targetSquare > sourceSquare) ? (targetSquare - 1) : (targetSquare + 1);


    position->boards[rookPiece] = BOARD_SQUARE_POP(position->boards[rookPiece], rookSourceSquare);
    
    position->boards[sourcePiece] = BOARD_SQUARE_SET(position->boards[sourcePiece], targetSquare);

    position->boards[rookPiece] = BOARD_SQUARE_SET(position->boards[rookPiece], rookTargetSquare);
  }
  else if(move & MOVE_MASK_PASSANT) // Passant
  {
    Square passantSquare = (sourcePiece == PIECE_WHITE_PAWN) ? (targetSquare + BOARD_FILES) : (targetSquare - BOARD_FILES);

    Piece passantPiece = (sourcePiece == PIECE_WHITE_PAWN) ? PIECE_BLACK_PAWN : PIECE_WHITE_PAWN;

    position->boards[sourcePiece] = BOARD_SQUARE_SET(position->boards[sourcePiece], targetSquare);

    position->boards[passantPiece] = BOARD_SQUARE_POP(position->boards[passantPiece], passantSquare);
  }
  else if((sourcePiece == PIECE_WHITE_PAWN && targetSquare >= A8 && targetSquare <= H8) ||
          (sourcePiece == PIECE_BLACK_PAWN && targetSquare >= A1 && targetSquare <= H1)) // Promote
  {
    Piece promotePiece = MOVE_GET_PROMOTE(move);

    position->boards[promotePiece] = BOARD_SQUARE_SET(position->boards[promotePiece], targetSquare);
  }
  else // Default
  {
    position->boards[sourcePiece] = BOARD_SQUARE_SET(position->boards[sourcePiece], targetSquare);

    if(sourcePiece == PIECE_WHITE_ROOK || sourcePiece == PIECE_BLACK_ROOK)
    {
      if(sourceSquare == A1) position->castle &= ~CASTLE_WHITE_QUEEN;

      else if(sourceSquare == H1) position->castle &= ~CASTLE_WHITE_KING;

      else if(sourceSquare == A8) position->castle &= ~CASTLE_BLACK_QUEEN;

      else if(sourceSquare == H8) position->castle &= ~CASTLE_BLACK_KING;
    }
  }
}

bool ident_capture_move(U64 boards[12], Square targetSquare)
{
  Piece targetPiece = boards_square_piece(boards, targetSquare);

  return (targetPiece != PIECE_NONE);
}

bool ident_double_move(Piece sourcePiece, Square sourceSquare, Square targetSquare)
{
  if(sourcePiece == PIECE_WHITE_PAWN)
  {
    if((sourceSquare - targetSquare) == (BOARD_FILES * 2)) return true;
  }
  else if(sourcePiece == PIECE_BLACK_PAWN)
  {
    if((targetSquare - sourceSquare) == (BOARD_FILES * 2)) return true;
  }
  return false;
}

bool ident_castle_move(Piece sourcePiece, Square sourceSquare, Square targetSquare)
{
  int squareDiff = (targetSquare - sourceSquare);

  if((sourcePiece != PIECE_WHITE_KING) && (sourcePiece != PIECE_BLACK_KING)) return false;
  
  return ((squareDiff == +2) || (squareDiff == -2));
}

bool ident_passant_move(U64 boards[12], Piece sourcePiece, Square sourceSquare, Square targetSquare)
{
  Piece targetPiece = boards_square_piece(boards, targetSquare);

  if(targetPiece != PIECE_NONE) return false;

  int squareDiff = (targetSquare - sourceSquare);

  if(sourcePiece == PIECE_WHITE_PAWN)
  {
    if((squareDiff == -9) || (squareDiff == -7)) return true;
  }
  else if(sourcePiece == PIECE_BLACK_PAWN)
  {
    if((squareDiff == +9) || (squareDiff == +7)) return true;
  }
  return false;
}

U64 complete_board_move(U64 boards[12], Move move)
{
  U64 completeMove = move;

  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);


  Piece sourcePiece = boards_square_piece(boards, sourceSquare);

  completeMove |= MOVE_SET_PIECE(sourcePiece);


  if((sourcePiece == PIECE_WHITE_PAWN) && (targetSquare >= A8) && (targetSquare <= H8))
  {
    completeMove |= MOVE_SET_PROMOTE(PIECE_WHITE_QUEEN);
  }
  if((sourcePiece == PIECE_BLACK_PAWN) && (targetSquare >= A1) && (targetSquare <= H1))
  {
    completeMove |= MOVE_SET_PROMOTE(PIECE_BLACK_QUEEN);
  }

  if(ident_capture_move(boards, targetSquare)) completeMove |= MOVE_MASK_CAPTURE;
  
  if(ident_double_move(sourcePiece, sourceSquare, targetSquare)) completeMove |= MOVE_MASK_DOUBLE;

  if(ident_castle_move(sourcePiece, sourceSquare, targetSquare)) completeMove |= MOVE_MASK_CASTLE;

  if(ident_passant_move(boards, sourcePiece, sourceSquare, targetSquare)) completeMove |= MOVE_MASK_PASSANT;

  return completeMove;
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

int main(int argc, char* argv[])
{
  printf("\n[ treestump ]\n\n");

  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();


  init_board_lookup_lines();



  Position position;

  parse_fen_string(&position, "rnbqkbnr/ppppp3/8/5Pp1/8/8/PPPPP1PP/R3K2R w KQkq - 0 1");


  char inputMove[64];

  for(int index = 0; index < 10; index++)
  {
    position_print(position);

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

  return 0;
}