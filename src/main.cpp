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
    // A rook checks the king along an open file; a piece blocks the check.
{
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4);  // e1
    board.putPiece(Piece::BlackRook, 60); // e8

    assert(board.isInCheck(Color::White));

    board.putPiece(Piece::WhitePawn, 28); // e4 blocks the rook
    assert(!board.isInCheck(Color::White));
}

  assert(!Board{}.isSquareAttacked(-1, Color::White));
  assert(!Board{}.isSquareAttacked(64, Color::Black));

  // A pinned rook cannot move away and expose its king to check.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4);  // e1
    board.putPiece(Piece::WhiteRook, 12); // e2, blocking the e-file
    board.putPiece(Piece::BlackRook, 60); // e8

    assert(!board.isLegalMove(12, 13)); // e2 to f2 exposes the king
    board.movePiece(12, 13);

    assert(board.pieceAt(12) == Piece::WhiteRook);
    assert(board.pieceAt(13) == Piece::None);
    assert(board.sideToMove == Color::White);
  }

  // The king cannot move onto a square attacked by a rook.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4);  // e1
    board.putPiece(Piece::BlackRook, 60); // e8

    assert(!board.isLegalMove(4, 12)); // e1 to e2 is still on the rook's file
    board.movePiece(4, 12);

    assert(board.pieceAt(4) == Piece::WhiteKing);
    assert(board.pieceAt(12) == Piece::None);
    assert(board.sideToMove == Color::White);
  }

  // The king can escape check by moving to a safe square.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4);  // e1
    board.putPiece(Piece::BlackRook, 60); // e8

    assert(board.isInCheck(Color::White));
    assert(board.isLegalMove(4, 3)); // e1 to d1
    board.movePiece(4, 3);

    assert(board.pieceAt(4) == Piece::None);
    assert(board.pieceAt(3) == Piece::WhiteKing);
    assert(!board.isInCheck(Color::White));
    assert(board.sideToMove == Color::Black);
  }
// White pawn promotes on the eighth rank.
{
    Board board;
    board.clear();
    board.putPiece(Piece::WhitePawn, 48); // a7
    board.movePiece(48, 56, Piece::WhiteQueen); // a7 to a8

    assert(board.pieceAt(48) == Piece::None);
    assert(board.pieceAt(56) == Piece::WhiteQueen);
    assert(board.sideToMove == Color::Black);
}

// Black pawn promotes on the first rank.
{
    Board board;
    board.clear();
    board.sideToMove = Color::Black;
    board.putPiece(Piece::BlackPawn, 8); // a2
    board.movePiece(8, 0, Piece::BlackQueen); // a2 to a1

    assert(board.pieceAt(8) == Piece::None);
    assert(board.pieceAt(0) == Piece::BlackQueen);
    assert(board.sideToMove == Color::White);
}

// White can underpromote to a knight.
{
    Board board;
    board.clear();
    board.putPiece(Piece::WhitePawn, 48); // a7
    board.movePiece(48, 56, Piece::WhiteKnight);

    assert(board.pieceAt(48) == Piece::None);
    assert(board.pieceAt(56) == Piece::WhiteKnight);
    assert(board.sideToMove == Color::Black);
}

// Promotion without a choice is rejected.
{
    Board board;
    board.clear();
    board.putPiece(Piece::WhitePawn, 48); // a7
    board.movePiece(48, 56);

    assert(board.pieceAt(48) == Piece::WhitePawn);
    assert(board.pieceAt(56) == Piece::None);
    assert(board.sideToMove == Color::White);
}

// A pawn cannot promote to a piece of the other color.
{
    Board board;
    board.clear();
    board.putPiece(Piece::WhitePawn, 48); // a7
    board.movePiece(48, 56, Piece::BlackQueen);

    assert(board.pieceAt(48) == Piece::WhitePawn);
    assert(board.pieceAt(56) == Piece::None);
    assert(board.sideToMove == Color::White);
}

// En passant is available immediately after an opposing pawn moves two squares.
{
    Board board;
    board.clear();
    board.putPiece(Piece::WhitePawn, 36); // e5
    board.putPiece(Piece::BlackPawn, 51); // d7
    board.sideToMove = Color::Black;

    board.movePiece(51, 35); // d7 to d5; en passant target is d6 (43)
    assert(board.enPassantSquare == 43);
    assert(board.pieceAt(35) == Piece::BlackPawn);

    board.movePiece(36, 43); // e5 captures en passant on d6
    assert(board.pieceAt(36) == Piece::None);
    assert(board.pieceAt(35) == Piece::None); // captured pawn removed from d5
    assert(board.pieceAt(43) == Piece::WhitePawn);
    assert(board.enPassantSquare == -1);
    assert(board.sideToMove == Color::Black);
}

  // White kingside castling moves both king and rook.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4); // e1
    board.putPiece(Piece::WhiteRook, 7); // h1
    board.putPiece(Piece::BlackKing, 60); // e8
    board.castlingRights = WhiteKingSide;

    assert(board.isLegalMove(4, 6));
    board.movePiece(4, 6);
    assert(board.pieceAt(4) == Piece::None);
    assert(board.pieceAt(6) == Piece::WhiteKing);
    assert(board.pieceAt(7) == Piece::None);
    assert(board.pieceAt(5) == Piece::WhiteRook);
    assert((board.castlingRights & WhiteKingSide) == 0);
  }

  // White queenside castling moves the rook from a1 to d1.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4); // e1
    board.putPiece(Piece::WhiteRook, 0); // a1
    board.putPiece(Piece::BlackKing, 60); // e8
    board.castlingRights = WhiteQueenSide;

    assert(board.isLegalMove(4, 2));
    board.movePiece(4, 2);
    assert(board.pieceAt(2) == Piece::WhiteKing);
    assert(board.pieceAt(3) == Piece::WhiteRook);
    assert(board.pieceAt(0) == Piece::None);
  }

  // Black kingside castling.
  {
    Board board;
    board.clear();
    board.sideToMove = Color::Black;
    board.putPiece(Piece::WhiteKing, 4); // e1
    board.putPiece(Piece::BlackKing, 60); // e8
    board.putPiece(Piece::BlackRook, 63); // h8
    board.castlingRights = BlackKingSide;

    assert(board.isLegalMove(60, 62));
    board.movePiece(60, 62);
    assert(board.pieceAt(62) == Piece::BlackKing);
    assert(board.pieceAt(61) == Piece::BlackRook);
    assert(board.pieceAt(63) == Piece::None);
  }

  // Black queenside castling.
  {
    Board board;
    board.clear();
    board.sideToMove = Color::Black;
    board.putPiece(Piece::WhiteKing, 4); // e1
    board.putPiece(Piece::BlackKing, 60); // e8
    board.putPiece(Piece::BlackRook, 56); // a8
    board.castlingRights = BlackQueenSide;

    assert(board.isLegalMove(60, 58));
    board.movePiece(60, 58);
    assert(board.pieceAt(58) == Piece::BlackKing);
    assert(board.pieceAt(59) == Piece::BlackRook);
    assert(board.pieceAt(56) == Piece::None);
  }

  // Castling is rejected if a path square is occupied or attacked.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4);
    board.putPiece(Piece::WhiteRook, 7);
    board.putPiece(Piece::WhiteBishop, 5); // blocks f1
    board.castlingRights = WhiteKingSide;
    assert(!board.isLegalMove(4, 6));

    board.clear();
    board.putPiece(Piece::WhiteKing, 4);
    board.putPiece(Piece::WhiteRook, 7);
    board.putPiece(Piece::BlackKing, 60);
    board.putPiece(Piece::BlackRook, 61); // attacks f1, the transit square
    board.castlingRights = WhiteKingSide;
    assert(!board.isLegalMove(4, 6));

    board.clear();
    board.putPiece(Piece::WhiteKing, 4);
    board.putPiece(Piece::WhiteRook, 7);
    board.putPiece(Piece::BlackKing, 60);
    board.putPiece(Piece::BlackRook, 62); // attacks g1, the destination
    board.castlingRights = WhiteKingSide;
    assert(!board.isLegalMove(4, 6));
  }

  // A rook moving from or being captured on its original square removes that
  // side's corresponding castling right.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4);
    board.putPiece(Piece::WhiteRook, 7);
    board.putPiece(Piece::BlackKing, 60);
    board.castlingRights = WhiteKingSide;

    board.movePiece(7, 15); // h1 to h2
    assert((board.castlingRights & WhiteKingSide) == 0);

    board.clear();
    board.sideToMove = Color::Black;
    board.putPiece(Piece::WhiteKing, 4);
    board.putPiece(Piece::BlackKing, 60);
    board.putPiece(Piece::WhiteRook, 7); // h1
    board.putPiece(Piece::BlackRook, 63); // h8
    board.castlingRights = WhiteKingSide;

    board.movePiece(63, 7); // Black rook captures the rook on h1.
    assert(board.pieceAt(7) == Piece::BlackRook);
    assert((board.castlingRights & WhiteKingSide) == 0);
  }

  // Legal move generation matches the standard starting-position perft counts.
  {
    Board board;
    board.setStartingPosition();
    assert(board.generateLegalMoves().size() == 20);
    assert(board.perft(1) == 20);
    assert(board.perft(2) == 400);
    assert(board.perft(3) == 8902);
  }

  // A promotion move is represented as four distinct legal choices.
  {
    Board board;
    board.clear();
    board.putPiece(Piece::WhiteKing, 4);
    board.putPiece(Piece::BlackKing, 60);
    board.putPiece(Piece::WhitePawn, 48); // a7
    int promotionMoves = 0;
    for (const Move& move : board.generateLegalMoves()) {
      if (move.from == 48 && move.to == 56) {
        ++promotionMoves;
      }
    }
    assert(promotionMoves == 4);
  }

  std::cout << "All move, attack, king-safety, castling, and move-generation checks passed.\n";
}
