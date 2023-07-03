#include "../engine.h"

void qswap_array_moves(Move* moves, int index1, int index2)
{
  Move tempMove = moves[index1];
  moves[index1] = moves[index2];
  moves[index2] = tempMove;
}

void qswap_move_scores(int* scores, int index1, int index2)
{
  int tempScore = scores[index1];
  scores[index1] = scores[index2];
  scores[index2] = tempScore;
}

void qswap_moves_scores(Move* moves, int* scores, int index1, int index2)
{
  qswap_array_moves(moves, index1, index2);

  qswap_move_scores(scores, index1, index2);
}

int partly_qsort_moves(Move* moves, int* scores, int index1, int index2)
{
  int pivotScore = scores[index2];

  int iIndex = (index1 - 1);

  for(int jIndex = index1; jIndex <= (index2 - 1); jIndex += 1)
  {
    if(scores[jIndex] < pivotScore) continue;

    qswap_moves_scores(moves, scores, (++iIndex), jIndex);
  }
  qswap_moves_scores(moves, scores, (iIndex + 1), index2);

  return (iIndex + 1);
}

void qsort_moves_indexes(Move* moves, int* scores, int index1, int index2)
{
  if(index1 >= index2) return;

  int partIndex = partly_qsort_moves(moves, scores, index1, index2);

  qsort_moves_indexes(moves, scores, index1, (partIndex - 1));

  qsort_moves_indexes(moves, scores, (partIndex + 1), index2);
}

void qsort_moves_scores(MoveArray* moveArray, int* scores)
{
  qsort_moves_indexes(moveArray->moves, scores, 0, (moveArray->amount - 1));
}

int guess_move_score(Position position, Move move)
{
  int score = 0;

  Square targetSquare = MOVE_GET_TARGET(move);
  Piece targetPiece = boards_square_piece(position.boards, targetSquare);

  if(targetPiece != PIECE_NONE)
  {
    Piece sourcePiece = MOVE_GET_PIECE(move);

    int sourcePieceScore = PIECE_SCORES[sourcePiece];
    int targetPieceScore = PIECE_SCORES[targetPiece];

    score += ((10 * sourcePieceScore) - targetPieceScore);
  }

  Piece promotePiece = MOVE_GET_PROMOTE(move);

  if(promotePiece != PIECE_WHITE_PAWN)
  {
    score += PIECE_SCORES[promotePiece];
  }

  return (position.side == SIDE_WHITE) ? score : -score;
}

void guess_move_scores(int* scores, Position position, MoveArray moveArray)
{
  for(int index = 0; index < moveArray.amount; index++)
  {
    scores[index] = guess_move_score(position, moveArray.moves[index]);
  }
}

void guess_order_moves(MoveArray* moveArray, Position position)
{
  int scores[moveArray->amount];

  guess_move_scores(scores, position, *moveArray);

  qsort_moves_scores(moveArray, scores);
}