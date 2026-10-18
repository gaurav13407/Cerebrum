#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include <iosfwd>
#include <string>

enum class Color : std::uint8_t { White = 0, Black = 1 };

enum class Piece : std::uint8_t {
  WhitePawn = 0,
  WhiteKnight,
  WhiteBishop,
  WhiteRook,
  WhiteQueen,
  WhiteKing,

  BlackPawn,
  BlackKnight,
  BlackBishop,
  BlackRook,
  BlackQueen,
  BlackKing,

  None = 255
};

enum CastlingRight : std::uint8_t{
    WhiteKingSide =1 <<0,
    WhiteQueenSide = 1 <<1,
    BlackKingSide =1 <<2,
    BlackQueenSide=1 <<3,
};

struct Move {
  int from = -1;
  int to = -1;
  Piece promotion = Piece::None;
};

struct Board {
  // One bitboard per peice type and color
  std::array<std::uint64_t, 12> pieces{};

  // Combined occuancy bitboard, useful for move generation
  std::array<std::uint64_t, 2> occupancy{};
  Color sideToMove = Color::White;
  int enPassantSquare = -1; // Target square for the immediately available en passant capture.

  void clear();
  void setStartingPosition();
  void print() const;

  // Place and remove peice
  void putPiece(Piece piece, int square);
  void removePiece(Piece peice, int square);
  Piece pieceAt(int square) const;

  // Move Peice
  void movePiece(int from, int to, Piece promotion=Piece::None);
  bool isPseudoLegalMove(int from, int to) const;
  bool isSquareAttacked(int square, Color byColor) const;
  bool isInCheck(Color color) const;
  bool isLegalMove(int from,int to, Piece promotion=Piece::None) const;
  std::vector<Move> generateLegalMoves() const;
  std::uint64_t perft(int depth);
  void perftDivide(int depth) const;
  bool loadFEN(const std::string &fen);

  // Castling Rights 
  std::uint8_t castlingRights = 0;
};
