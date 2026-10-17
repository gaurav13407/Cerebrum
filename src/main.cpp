#include "board.h"

#include <cassert>
#include <iostream>

int main() {
  // One-square pawn move: a2 to a3.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(8, 16);

    assert(board.pieceAt(8) == Piece::None);
    assert(board.pieceAt(16) == Piece::WhitePawn);
    assert(board.sideToMove == Color::Black);
  }

  // Two-square pawn move from its starting rank: a2 to a4.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(8, 24);

    assert(board.pieceAt(8) == Piece::None);
    assert(board.pieceAt(24) == Piece::WhitePawn);
  }

  // Illegal three-square move: a2 to a5.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(8, 32);

    assert(board.pieceAt(8) == Piece::WhitePawn);
    assert(board.pieceAt(32) == Piece::None);
    assert(board.sideToMove == Color::White);
  }

  // Diagonal move to an empty square: a2 to b3.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(8, 17);

    assert(board.pieceAt(8) == Piece::WhitePawn);
    assert(board.pieceAt(17) == Piece::None);
  }

  // Capture: e2-e4, d7-d5, then e4 captures on d5.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(12, 28);
    board.movePiece(51, 35);
    board.movePiece(28, 35);

    assert(board.pieceAt(28) == Piece::None);
    assert(board.pieceAt(35) == Piece::WhitePawn);
    assert(board.sideToMove == Color::Black);
  }
  // Knight jumps from b1 to c3.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(1, 18);

    assert(board.pieceAt(1) == Piece::None);
    assert(board.pieceAt(18) == Piece::WhiteKnight);
    assert(board.sideToMove == Color::Black);
  }

  // Bishop moves diagonally when the path is clear.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteBishop, 2); // c1
    board.movePiece(2, 20);                // c1 to e3

    assert(board.pieceAt(2) == Piece::None);
    assert(board.pieceAt(20) == Piece::WhiteBishop);
    assert(board.sideToMove == Color::Black);
  }

  // Bishop cannot jump over the pawn on d2.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(2, 20); // c1 to e3

    assert(board.pieceAt(2) == Piece::WhiteBishop);
    assert(board.pieceAt(11) == Piece::WhitePawn); // d2 still occupied
    assert(board.pieceAt(20) == Piece::None);
    assert(board.sideToMove == Color::White);
  }

  // Rook moves straight when the path is clear.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteRook, 0); // a1
    board.movePiece(0, 24);              // a1 to a4

    assert(board.pieceAt(0) == Piece::None);
    assert(board.pieceAt(24) == Piece::WhiteRook);
    assert(board.sideToMove == Color::Black);
  }

  // Rook cannot jump over the pawn on a2.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(0, 24); // a1 to a4

    assert(board.pieceAt(0) == Piece::WhiteRook);
    assert(board.pieceAt(8) == Piece::WhitePawn); // a2 still occupied
    assert(board.pieceAt(24) == Piece::None);
    assert(board.sideToMove == Color::White);
  }
  // Queen moves straight when the path is clear: d4 to d7.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteQueen, 27); // d4
    board.movePiece(27, 51);               // d4 to d7

    assert(board.pieceAt(27) == Piece::None);
    assert(board.pieceAt(51) == Piece::WhiteQueen);
    assert(board.sideToMove == Color::Black);
  }

  // Queen moves diagonally when the path is clear: d4 to g7.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteQueen, 27); // d4
    board.movePiece(27, 54);               // d4 to g7

    assert(board.pieceAt(27) == Piece::None);
    assert(board.pieceAt(54) == Piece::WhiteQueen);
  }

  // Queen cannot jump over the pawn on d2.
  {
    Board board;
    board.setStartingPosition();
    board.movePiece(3, 27); // d1 to d4

    assert(board.pieceAt(3) == Piece::WhiteQueen);
    assert(board.pieceAt(11) == Piece::WhitePawn); // d2 still occupied
    assert(board.pieceAt(27) == Piece::None);
    assert(board.sideToMove == Color::White);
  }

  // King moves one square: e1 to e2.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4); // e1
    board.movePiece(4, 12);              // e1 to e2

    assert(board.pieceAt(4) == Piece::None);
    assert(board.pieceAt(12) == Piece::WhiteKing);
    assert(board.sideToMove == Color::Black);
  }

  // King cannot move two squares: e1 to e3.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4); // e1
    board.movePiece(4, 20);              // e1 to e3

    assert(board.pieceAt(4) == Piece::WhiteKing);
    assert(board.pieceAt(20) == Piece::None);
    assert(board.sideToMove == Color::White);
  }

   // White pawn on e4 attacks d5 and f5, but not e5.
{
    Board board;
    board.clear();
    board.putPiece(Piece::WhitePawn, 28); // e4

    assert(board.isSquareAttacked(35, Color::White));  // d5
    assert(board.isSquareAttacked(37, Color::White));  // f5
    assert(!board.isSquareAttacked(36, Color::White)); // e5
}

// Black pawn on e5 attacks d4 and f4, but not e4.
{
    Board board;
    board.clear();
    board.putPiece(Piece::BlackPawn, 36); // e5

    assert(board.isSquareAttacked(27, Color::Black));  // d4
    assert(board.isSquareAttacked(29, Color::Black));  // f4
    assert(!board.isSquareAttacked(28, Color::Black)); // e4
}

  // Knight, king, bishop, rook, and queen attack detection.
  {
    Board board;

    board.clear();
    board.putPiece(Piece::WhiteKnight, 18); // c3
    assert(board.isSquareAttacked(1, Color::White));
    assert(board.isSquareAttacked(35, Color::White));
    assert(!board.isSquareAttacked(26, Color::White));

    board.clear();
    board.putPiece(Piece::WhiteKing, 28); // e4
    assert(board.isSquareAttacked(35, Color::White));
    assert(!board.isSquareAttacked(44, Color::White));

    board.clear();
    board.putPiece(Piece::WhiteBishop, 2); // c1
    assert(board.isSquareAttacked(38, Color::White)); // g5
    board.putPiece(Piece::WhitePawn, 11); // d2 blocks the diagonal
    assert(!board.isSquareAttacked(38, Color::White));

    board.clear();
    board.putPiece(Piece::WhiteRook, 0); // a1
    assert(board.isSquareAttacked(56, Color::White)); // a8
    board.putPiece(Piece::BlackPawn, 24); // a4 blocks the rook
    assert(!board.isSquareAttacked(56, Color::White));

    board.clear();
    board.putPiece(Piece::WhiteQueen, 27); // d4
    assert(board.isSquareAttacked(59, Color::White)); // d8, straight
    assert(board.isSquareAttacked(63, Color::White)); // h8, diagonal
  }

  assert(!Board{}.isSquareAttacked(-1, Color::White));
  assert(!Board{}.isSquareAttacked(64, Color::Black));

  std::cout << "All move and attack checks passed.\n";
}
