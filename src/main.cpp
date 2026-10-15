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

    std::cout << "All pawn move checks passed.\n";
}
