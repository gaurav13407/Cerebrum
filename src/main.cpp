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
    board.putPiece(Piece::WhiteBishop, 2);  // c1
    board.movePiece(2, 20);                 // c1 to e3

    assert(board.pieceAt(2) == Piece::None);
    assert(board.pieceAt(20) == Piece::WhiteBishop);
    assert(board.sideToMove == Color::Black);
}

// Bishop cannot jump over the pawn on d2.
{
    Board board;
    board.setStartingPosition();
    board.movePiece(2, 20);                 // c1 to e3

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
    board.movePiece(0, 24);             // a1 to a4

    assert(board.pieceAt(0) == Piece::None);
    assert(board.pieceAt(24) == Piece::WhiteRook);
    assert(board.sideToMove == Color::Black);
}

// Rook cannot jump over the pawn on a2.
{
    Board board;
    board.setStartingPosition();
    board.movePiece(0, 24);             // a1 to a4

    assert(board.pieceAt(0) == Piece::WhiteRook);
    assert(board.pieceAt(8) == Piece::WhitePawn); // a2 still occupied
    assert(board.pieceAt(24) == Piece::None);
    assert(board.sideToMove == Color::White);
}

    std::cout << "All move checks passed.\n";
}
