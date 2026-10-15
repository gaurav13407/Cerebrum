#include "board.h"

#include <cstdint>
#include <iostream>

namespace  {
    constexpr std::uint64_t bit(int square){
        return std::uint64_t{1} << square;
    }

    constexpr int pieceIndex(Piece piece){
        return static_cast<int>(piece);
    }

    constexpr Color colorOf(Piece piece){
        return pieceIndex(piece) < 6 ? Color::White : Color::Black;
    }
    char pieceCharacter(Piece piece) {
    switch (piece) {
        case Piece::WhitePawn:   return 'P';
        case Piece::WhiteKnight: return 'N';
        case Piece::WhiteBishop: return 'B';
        case Piece::WhiteRook:   return 'R';
        case Piece::WhiteQueen:  return 'Q';
        case Piece::WhiteKing:   return 'K';
        case Piece::BlackPawn:   return 'p';
        case Piece::BlackKnight: return 'n';
        case Piece::BlackBishop: return 'b';
        case Piece::BlackRook:   return 'r';
        case Piece::BlackQueen:  return 'q';
        case Piece::BlackKing:   return 'k';
        case Piece::None:        return '.';
    }
    return '.';
}
} 
void Board::clear() {
    pieces.fill(0);
    occupancy.fill(0);
    sideToMove = Color::White;
}

void Board::putPiece(Piece piece, int square) {
    if (piece == Piece::None || square < 0 || square >= 64) {
        return;
    }

    const std::uint64_t squareBit = bit(square);
    pieces[pieceIndex(piece)] |= squareBit;
    occupancy[static_cast<int>(colorOf(piece))] |= squareBit;
}

void Board::removePiece(Piece piece, int square) {
    if (piece == Piece::None || square < 0 || square >= 64) {
        return;
    }

    const std::uint64_t squareBit = bit(square);
    pieces[pieceIndex(piece)] &= ~squareBit;
    occupancy[static_cast<int>(colorOf(piece))] &= ~squareBit;
}

    void Board::setStartingPosition() {
    clear();

    // White back rank: a1 through h1.
    putPiece(Piece::WhiteRook,   0);
    putPiece(Piece::WhiteKnight, 1);
    putPiece(Piece::WhiteBishop, 2);
    putPiece(Piece::WhiteQueen,  3);
    putPiece(Piece::WhiteKing,   4);
    putPiece(Piece::WhiteBishop, 5);
    putPiece(Piece::WhiteKnight, 6);
    putPiece(Piece::WhiteRook,   7);

    // White pawns: a2 through h2.
    for (int square = 8; square < 16; ++square) {
        putPiece(Piece::WhitePawn, square);
    }

    // Black pawns: a7 through h7.
    for (int square = 48; square < 56; ++square) {
        putPiece(Piece::BlackPawn, square);
    }

    // Black back rank: a8 through h8.
    putPiece(Piece::BlackRook,   56);
    putPiece(Piece::BlackKnight, 57);
    putPiece(Piece::BlackBishop, 58);
    putPiece(Piece::BlackQueen,  59);
    putPiece(Piece::BlackKing,   60);
    putPiece(Piece::BlackBishop, 61);
    putPiece(Piece::BlackKnight, 62);
    putPiece(Piece::BlackRook,   63);
    }
    void Board::print() const {
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " ";

        for (int file = 0; file < 8; ++file) {
            const int square = rank * 8 + file;
            char shown = '.';

            for (int p = 0; p < 12; ++p) {
                if ((pieces[p] & bit(square)) != 0) {
                    shown = pieceCharacter(static_cast<Piece>(p));
                    break;
                }
            }

            std::cout << shown << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h\n";
        
    }

    
