#pragma once
#include <array>
#include <cstdint>

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

struct Board {
  // One bitboard per peice type and color
  std::array<std::uint64_t, 12> pieces{};

  // Combined occuancy bitboard, useful for move generation
  std::array<std::uint64_t, 2> occupancy{};

  Color sideToMove = Color::White;

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
};
