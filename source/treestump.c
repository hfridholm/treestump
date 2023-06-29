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

bool pawn_white_double_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  if(!(sourceSquare >= A2 && sourceSquare <= H2)) return false;

  if((sourceSquare - targetSquare) != (BOARD_FILES * 2)) return false;

  U64 moveCovers = (1ULL << targetSquare) | (1ULL << (targetSquare + BOARD_FILES));

  if(moveCovers & position.covers[SIDE_BOTH]) return false;

  return true;
}

bool pawn_black_double_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  if(!(sourceSquare >= A7 && sourceSquare <= H7)) return false;

  if((targetSquare - sourceSquare) != (BOARD_FILES * 2)) return false;

  U64 moveCovers = (1ULL << targetSquare) | (1ULL << (targetSquare - BOARD_FILES));

  if(moveCovers & position.covers[SIDE_BOTH]) return false;

  return true;
}

bool pawn_double_pseudo_legal(Position position, Move move)
{
  Piece sourcePiece = MOVE_GET_PIECE(move);

  if(sourcePiece == PIECE_WHITE_PAWN)
  {
    return pawn_white_double_pseudo_legal(position, move);
  }
  else if(sourcePiece == PIECE_BLACK_PAWN)
  {
    return pawn_black_double_pseudo_legal(position, move);
  }
  else return false;
}

bool pawn_white_passant_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  int squareDiff = (targetSquare - sourceSquare);

  if(!((sourceSquare >= A5) && (sourceSquare <= H5))) return false;

  if(!((targetSquare >= A6) && (targetSquare <= H6))) return false;

  if((squareDiff != -9) && (squareDiff != -7)) return false;

  if(!BOARD_SQUARE_GET(position.boards[PIECE_BLACK_PAWN], (targetSquare + BOARD_FILES))) return false;

  if(boards_square_piece(position.boards, targetSquare) != PIECE_NONE) return false;

  return true;
}

bool pawn_black_passant_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  int squareDiff = (targetSquare - sourceSquare);

  if(!((sourceSquare >= A4) && (sourceSquare <= H4))) return false;

  if(!((targetSquare >= A3) && (targetSquare <= H3))) return false;

  if((squareDiff != +9) && (squareDiff != +7)) return false;

  if(!BOARD_SQUARE_GET(position.boards[PIECE_BLACK_PAWN], (targetSquare - BOARD_FILES))) return false;

  if(boards_square_piece(position.boards, targetSquare) != PIECE_NONE) return false;

  return true;
}

bool pawn_passant_pseudo_legal(Position position, Move move)
{
  Piece sourcePiece = MOVE_GET_PIECE(move);

  if(sourcePiece == PIECE_WHITE_PAWN)
  {
    return pawn_white_passant_pseudo_legal(position, move);
  }
  else if(sourcePiece == PIECE_BLACK_PAWN)
  {
    return pawn_black_passant_pseudo_legal(position, move);
  }
  else return false;
}

bool pawn_capture_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);


  if(!((1ULL << targetSquare) & position.covers[SIDE_BOTH])) return false;

  bool targetWhite = ((1ULL << targetSquare) & position.covers[SIDE_WHITE]);
  bool sourceWhite = ((1ULL << sourceSquare) & position.covers[SIDE_WHITE]);

  if(!(sourceWhite ^ targetWhite)) return false;


  return (piece_lookup_attacks(position, sourceSquare) & (1ULL << targetSquare));
}

bool pawn_normal_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  if((1ULL << targetSquare) & position.covers[SIDE_BOTH]) return false;


  if(sourcePiece == PIECE_WHITE_PAWN)
  {
    if((sourceSquare - targetSquare) != BOARD_FILES) return false;
  }
  else if(sourcePiece == PIECE_BLACK_PAWN)
  {
    if((targetSquare - sourceSquare) != BOARD_FILES) return false;
  }
  else return false;

  return true;
}

bool pawn_move_pseudo_legal(Position position, Move move)
{
  if(move & MOVE_MASK_DOUBLE)
  {
    return pawn_double_pseudo_legal(position, move);
  }
  else if(move & MOVE_MASK_PASSANT)
  {
    return pawn_passant_pseudo_legal(position, move);
  }
  else if(move & MOVE_MASK_CAPTURE)
  {
    return pawn_capture_pseudo_legal(position, move);
  }
  else return pawn_normal_pseudo_legal(position, move);
}

bool castle_white_king_pseudo_legal(Position position)
{
  if(!BOARD_SQUARE_GET(position.boards[PIECE_WHITE_ROOK], H1)) return false;

  if(position.covers[SIDE_BOTH] & ((1ULL << G1) | (1ULL << F1))) return false;

  if(!(position.castle & CASTLE_WHITE_KING)) return false;

  return true;
}

bool castle_white_queen_pseudo_legal(Position position)
{
  if(!BOARD_SQUARE_GET(position.boards[PIECE_WHITE_ROOK], A1)) return false;

  if(position.covers[SIDE_BOTH] & ((1ULL << C1) | (1ULL << D1))) return false;

  if(!(position.castle & CASTLE_WHITE_QUEEN)) return false;

  return true;
}

bool castle_white_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  if(sourceSquare != E1) return false;

  if(targetSquare == G1) return castle_white_king_pseudo_legal(position);
  
  else if(targetSquare == C1) return castle_white_queen_pseudo_legal(position);
  
  else return false;
}

bool castle_black_king_pseudo_legal(Position position)
{
  if(!BOARD_SQUARE_GET(position.boards[PIECE_BLACK_ROOK], H1)) return false;

  if(position.covers[SIDE_BOTH] & ((1ULL << G8) | (1ULL << F8))) return false;

  if(!(position.castle & CASTLE_BLACK_KING)) return false;

  return true;
}

bool castle_black_queen_pseudo_legal(Position position)
{
  if(!BOARD_SQUARE_GET(position.boards[PIECE_BLACK_ROOK], A8)) return false;

  if(position.covers[SIDE_BOTH] & ((1ULL << C8) | (1ULL << D8))) return false;

  if(!(position.castle & CASTLE_BLACK_QUEEN)) return false;

  return true;
}

bool castle_black_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  if(sourceSquare != E8) return false;

  if(targetSquare == G8) return castle_black_king_pseudo_legal(position);
  
  else if(targetSquare == C8) return castle_black_queen_pseudo_legal(position);
  
  else return false;
}

bool castle_move_pseudo_legal(Position position, Move move)
{
  Piece sourcePiece = MOVE_GET_PIECE(move);

  if(sourcePiece == PIECE_WHITE_KING)
  {
    return castle_white_pseudo_legal(position, move);
  }
  else if(sourcePiece == PIECE_BLACK_KING)
  {
    return castle_black_pseudo_legal(position, move);
  }
  else return false;
}

bool normal_move_pseudo_legal(Position position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  // If the moving piece does not exists on the source square
  if(!BOARD_SQUARE_GET(position.boards[sourcePiece], sourceSquare)) return false;

  Piece targetPiece = boards_square_piece(position.boards, targetSquare);

  // If the capture flag and the target piece doesn't match each other
  if(((move & MOVE_MASK_CAPTURE) ? 1 : 0) ^ (targetPiece != PIECE_NONE)) return false;

  bool sourceWhite = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING);
  bool targetWhite = (targetPiece >= PIECE_WHITE_PAWN && targetPiece <= PIECE_WHITE_KING);

  // If source and target are not of different sides (same sides)
  if((move & MOVE_MASK_CAPTURE) && !(sourceWhite ^ targetWhite)) return false;

  // If there are any pieces between source and target square
  if(BOARD_LOOKUP_LINES[sourceSquare][targetSquare] & position.covers[SIDE_BOTH]) return false;

  // The piece is able to move from sourceSquare to targetSquare
  return (piece_lookup_attacks(position, sourceSquare) & (1ULL << targetSquare));
}

bool move_pseudo_legal(Position position, Move move)
{
  Piece sourcePiece = MOVE_GET_PIECE(move);

  if((sourcePiece == PIECE_WHITE_PAWN) || (sourcePiece == PIECE_WHITE_PAWN))
  {
    return pawn_move_pseudo_legal(position, move);
  }
  else if(move & MOVE_MASK_CASTLE)
  {
    return castle_move_pseudo_legal(position, move);
  }
  else return normal_move_pseudo_legal(position, move);
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

  // Change this to check side in position instead
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

  else if(sourcePiece == PIECE_BLACK_KING) position->castle &= ~(CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN);


  if(move & MOVE_MASK_DOUBLE)
  {
    if(sourcePiece == PIECE_WHITE_PAWN) position->passant = sourceSquare - BOARD_FILES;
    
    else if(sourcePiece == PIECE_BLACK_PAWN) position->passant = sourceSquare + BOARD_FILES;
  }
  else position->passant = SQUARE_NONE;

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

  parse_fen_string(&position, "rnbqkbnr/ppppp3/5P2/5Pp1/8/8/PPPPP1PP/R3K2R w KQkq - 0 1");


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