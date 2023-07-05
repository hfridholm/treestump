#include "../treestump.h"

extern Move create_double_move(Square sourceSquare, Square targetSquare, Piece piece);

extern Move create_promote_move(U64 boards[12], Square sourceSquare, Square targetSquare, Piece piece, Piece promotePiece);

extern Move create_castle_move(Square sourceSquare, Square targetSquare, Piece piece);

extern Move create_normal_move(U64 boards[12], Square sourceSquare, Square targetSquare, Piece piece);


extern bool move_fully_legal(Position position, Move move);

extern U64 piece_lookup_attacks(Position position, Square square);


extern int board_ls1b_index(U64 bitboard);


void create_white_pawn_promote_moves(MoveArray* moveArray, Position position, Square sourceSquare)
{
  for(Square targetSquare = (sourceSquare - 9); targetSquare <= (sourceSquare - 7); targetSquare++)
  {
    Move move = create_promote_move(position.boards, sourceSquare, targetSquare, PIECE_WHITE_PAWN, PIECE_WHITE_QUEEN);

    if(!move_fully_legal(position, move)) continue;

    for(Piece promotePiece = PIECE_WHITE_KNIGHT; promotePiece <= PIECE_WHITE_QUEEN; promotePiece++)
    {
      move = (move & ~MOVE_MASK_PROMOTE) | MOVE_SET_PROMOTE(promotePiece);

      moveArray->moves[moveArray->amount++] = move;
    }
  }
}

void create_white_pawn_moves(MoveArray* moveArray, Position position, Square sourceSquare)
{
  if(sourceSquare >= A7 && sourceSquare <= H7)
  {
    create_white_pawn_promote_moves(moveArray, position, sourceSquare);
  }
  else // Rewrite this code
  {
    for(Square targetSquare = (sourceSquare - 9); targetSquare <= (sourceSquare - 7); targetSquare++)
    {
      Move move = create_normal_move(position.boards, sourceSquare, targetSquare, PIECE_WHITE_PAWN);

      if(targetSquare == position.passant) move |= MOVE_MASK_PASSANT;

      if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;
    }

    if(sourceSquare >= A2 && sourceSquare <= H2)
    {
      Move move = create_double_move(sourceSquare, sourceSquare - (BOARD_FILES * 2), PIECE_WHITE_PAWN);

      if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;
    }
  }
}

void create_black_pawn_promote_moves(MoveArray* moveArray, Position position, Square sourceSquare)
{
  for(Square targetSquare = (sourceSquare + 7); targetSquare <= (sourceSquare + 9); targetSquare++)
  {
    Move move = create_promote_move(position.boards, sourceSquare, targetSquare, PIECE_BLACK_PAWN, PIECE_BLACK_QUEEN);

    if(!move_fully_legal(position, move)) continue;

    for(Piece promotePiece = PIECE_BLACK_KNIGHT; promotePiece <= PIECE_BLACK_QUEEN; promotePiece++)
    {
      move = (move & ~MOVE_MASK_PROMOTE) | MOVE_SET_PROMOTE(promotePiece);

      moveArray->moves[moveArray->amount++] = move;
    }
  }
}

void create_black_pawn_moves(MoveArray* moveArray, Position position, Square sourceSquare)
{
  if(sourceSquare >= A2 && sourceSquare <= H2)
  {
    create_black_pawn_promote_moves(moveArray, position, sourceSquare);
  }
  else // Rewrite this code
  {
    for(Square targetSquare = (sourceSquare + 7); targetSquare <= (sourceSquare + 9); targetSquare++)
    {
      Move move = create_normal_move(position.boards, sourceSquare, targetSquare, PIECE_BLACK_PAWN);

      if(targetSquare == position.passant) move |= MOVE_MASK_PASSANT;

      if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;
    }

    if(sourceSquare >= A7 && sourceSquare <= H7)
    {
      Move move = create_double_move(sourceSquare, sourceSquare + (BOARD_FILES * 2), PIECE_BLACK_PAWN);

      if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;
    }
  }
}

void create_white_castle_moves(MoveArray* moveArray, Position position)
{
  Move move = create_castle_move(E1, C1, PIECE_WHITE_KING);

  if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;

  move = create_castle_move(E1, G1, PIECE_WHITE_KING);

  if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;
}

void create_black_castle_moves(MoveArray* moveArray, Position position)
{
  Move move = create_castle_move(E8, C8, PIECE_BLACK_KING);

  if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;

  move = create_castle_move(E8, G8, PIECE_BLACK_KING);

  if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;
}

void create_white_normal_moves(MoveArray* moveArray, Position position, Square sourceSquare, Piece piece)
{
  if(piece == PIECE_WHITE_KING && sourceSquare == E1) create_white_castle_moves(moveArray, position);

  
  U64 attackBoard = piece_lookup_attacks(position, sourceSquare);

  while(attackBoard)
  {
    Square targetSquare = board_ls1b_index(attackBoard);

    Move move = create_normal_move(position.boards, sourceSquare, targetSquare, piece);
    
    if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;

    attackBoard = BOARD_SQUARE_POP(attackBoard, targetSquare);
  }
}

void create_white_moves(MoveArray* moveArray, Position position)
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_WHITE_KING; piece++)
  {
    U64 pieceBoard = position.boards[piece];

    while(pieceBoard)
    {
      Square sourceSquare = board_ls1b_index(pieceBoard);

      if(piece == PIECE_WHITE_PAWN) create_white_pawn_moves(moveArray, position, sourceSquare);

      else create_white_normal_moves(moveArray, position, sourceSquare, piece);

      pieceBoard = BOARD_SQUARE_POP(pieceBoard, sourceSquare);
    }
  }
}

void create_black_normal_moves(MoveArray* moveArray, Position position, Square sourceSquare, Piece piece)
{
  if(piece == PIECE_BLACK_KING && sourceSquare == E8) create_black_castle_moves(moveArray, position);
  

  U64 attackBoard = piece_lookup_attacks(position, sourceSquare);

  while(attackBoard)
  {
    Square targetSquare = board_ls1b_index(attackBoard);

    Move move = create_normal_move(position.boards, sourceSquare, targetSquare, piece);
    
    if(move_fully_legal(position, move)) moveArray->moves[moveArray->amount++] = move;

    attackBoard = BOARD_SQUARE_POP(attackBoard, targetSquare);
  }
}

void create_black_moves(MoveArray* moveArray, Position position)
{
  for(Piece piece = PIECE_BLACK_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    U64 pieceBoard = position.boards[piece];

    while(pieceBoard)
    {
      Square sourceSquare = board_ls1b_index(pieceBoard);

      if(piece == PIECE_BLACK_PAWN) create_black_pawn_moves(moveArray, position, sourceSquare);
      
      else create_black_normal_moves(moveArray, position, sourceSquare, piece);

      pieceBoard = BOARD_SQUARE_POP(pieceBoard, sourceSquare);
    }
  }
}

void create_moves(MoveArray* moveArray, Position position)
{
  if(position.side == SIDE_WHITE) create_white_moves(moveArray, position);
  
  else if(position.side == SIDE_BLACK) create_black_moves(moveArray, position);
}