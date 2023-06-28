#include "../engine.h"

#include "engine-values.h"
#include "string-handler.h"

bool parse_fen_side(Side* side, const char string[])
{
  int stringLength = strlen(string);

  if(stringLength != 1) return false;

  if(*string == 'w')
  {
    *side = SIDE_WHITE;
    return true;
  }
  else if(*string == 'b')
  {
    *side = SIDE_BLACK;
    return true;
  }
  return false;
}

bool parse_fen_clock(int* clock, const char string[])
{
  int number = atoi(string);

  if((number == 0) && (*string != '0')) return false;

  *clock = number;
  return true;
}

bool parse_fen_moves(int* moves, const char string[])
{
  int number = atoi(string);

  if((number == 0) && (*string != '0')) return false;

  *moves = number;
  return true;
}

bool parse_fen_enpassant(Square* enpassant, const char string[])
{
  if(!strcmp(string, "-"))
  {
    *enpassant = SQUARE_NONE;
    return true;
  }

  if(strlen(string) != 2) return false;


  int file = string[0] - 'a';
  int rank = BOARD_RANKS - (string[1] - '0');

  if(!(file >= 0 && file < BOARD_FILES) || !(rank >= 0 && rank < BOARD_RANKS)) return false;

  *enpassant = (rank * BOARD_FILES) + file;

  return true;
}

bool parse_fen_castle(Castle* castle, const char string[])
{
  if(!strcmp(string, "-")) return true;

  int stringLength = strlen(string);

  // Future Fix: Check for duplicate symbols and string length
  for(int index = 0; index < stringLength; index++)
  {
    switch(string[index])
    {
      case 'K':
        *castle |= CASTLE_WHITE_KING;
        break;

      case 'Q':
        *castle |= CASTLE_WHITE_QUEEN;
        break;

      case 'k':
        *castle |= CASTLE_BLACK_KING;
        break;

      case 'q':
        *castle |= CASTLE_BLACK_QUEEN;
        break;

      default: return false;
    }
  }
  return true;
}

bool parse_fen_bitboards(U64 bitboards[12], const char string[])
{
  char stringArray[8][128];

  int stringLength = strlen(string);

  if(!split_string_delim(stringArray, string, stringLength, "/", BOARD_RANKS)) return false;

  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    // Future Fix: check if rank length is less or equal to 8
    // and if there is any symbols that should not be in fen string
    int file = 0;

    for(int index = 0; index < strlen(stringArray[rank]); index++)
    {
      unsigned char symbol = stringArray[rank][index];


      Square square = (rank * BOARD_FILES) + file;

      if((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'))
      {
        Piece piece = SYMBOL_PIECES[symbol];

        bitboards[piece] = BITBOARD_SQUARE_SET(bitboards[piece], square);

        file++;
      }
      else if(symbol >= '0' && symbol <= '9')
      {
        file += (symbol - '0');
      }
      else return false;
    }
  }
  return true;
}

bool parse_fen_string(Position* position, const char fenString[])
{
  memset(position->bitboards, 0ULL, sizeof(position->bitboards));

  memset(position->occupancies, 0ULL, sizeof(position->occupancies));


  position->side = SIDE_WHITE;

  position->castle = 0;

  position->enpassant = SQUARE_NONE;


  int stringLength = strlen(fenString);

  char stringArray[6][128];

  if(!split_string_delim(stringArray, fenString, stringLength, " ", 6)) return false;


  if(!parse_fen_bitboards(position->bitboards, stringArray[0])) return false;

  if(!parse_fen_side(&position->side, stringArray[1])) return false;

  if(!parse_fen_castle(&position->castle, stringArray[2])) return false;

  if(!parse_fen_enpassant(&position->enpassant, stringArray[3])) return false;

  if(!parse_fen_clock(&position->clock, stringArray[4])) return false;

  if(!parse_fen_moves(&position->moves, stringArray[5])) return false;


  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_WHITE_KING; piece++)
  {
    position->occupancies[SIDE_WHITE] |= position->bitboards[piece];
  }

  for(Piece piece = PIECE_BLACK_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    position->occupancies[SIDE_BLACK] |= position->bitboards[piece];
  }

  position->occupancies[SIDE_BOTH] = position->occupancies[SIDE_WHITE] | position->occupancies[SIDE_BLACK];


  return true;
}