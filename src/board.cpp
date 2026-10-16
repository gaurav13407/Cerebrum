#include "board.h"
#include <cstdlib>
#include <cstdint>
#include <filesystem>
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

Piece Board::pieceAt(int square) const {
    if (square < 0 || square >= 64) {
        return Piece::None;
    }

    const std::uint64_t squareBit = bit(square);
    for (int p = 0; p < 12; ++p) {
        if ((pieces[p] & squareBit) != 0) {
            return static_cast<Piece>(p);
        }
    }
    return Piece::None;
}

bool Board::isPseudoLegalMove(int from, int to) const {
    if (from < 0 || from >= 64 || to < 0 || to >= 64 || from == to) {
        return false;
    }

    const Piece movingPiece = pieceAt(from);
    const Piece targetPiece = pieceAt(to);

    if (movingPiece == Piece::None || colorOf(movingPiece) != sideToMove) {
        return false;
    }

    if (targetPiece != Piece::None &&
        colorOf(movingPiece) == colorOf(targetPiece)) {
        return false;
    }

    // Knight move validateation
    if (movingPiece == Piece::WhiteKnight ||
        movingPiece == Piece::BlackKnight) {
        const int fileDistance = std::abs((to % 8) - (from % 8));
        const int rankDistance = std::abs((to / 8) - (from / 8));

        return (fileDistance == 1 && rankDistance == 2) ||
               (fileDistance == 2 && rankDistance == 1);
    }

    // Bishop move validation 
    if(movingPiece==Piece::WhiteBishop ||
            movingPiece==Piece::BlackBishop){
        const int fromFile=from%8;
        const int fromRank=from/8;
        const int toFile=to%8;
        const int toRank=to/8;

        const int fileDistance=std::abs(toFile-fromFile);
        const int rankDistance=std::abs(toRank-fromRank);
        if(fileDistance ==0 || fileDistance !=rankDistance){
            return false;
        }

        const int fileStep=(toFile>fromFile) ? 1:-1;
        const int rankStep=(toRank>fromRank) ? 1 :-1;
        int file=fileStep+fromFile;
        int rank=rankStep+fromRank;

        while(file!=toFile && rank!=toRank){
            if(pieceAt(rank*8+file)!=Piece::None){
                return false;
            }
            file+=fileStep;
            rank+=rankStep;
        }
        return true;
    } 
    //Rooks move validation 
    if (movingPiece == Piece::WhiteRook ||
    movingPiece == Piece::BlackRook) {
    const int fromFile = from % 8;
    const int fromRank = from / 8;
    const int toFile = to % 8;
    const int toRank = to / 8;

    // A rook must move along the same file or rank.
    if (fromFile != toFile && fromRank != toRank) {
        return false;
    }

    const int fileStep =
        (toFile > fromFile) ? 1 : (toFile < fromFile ? -1 : 0);
    const int rankStep =
        (toRank > fromRank) ? 1 : (toRank < fromRank ? -1 : 0);

    int file = fromFile + fileStep;
    int rank = fromRank + rankStep;

    while (file != toFile || rank != toRank) {
        if (pieceAt(rank * 8 + file) != Piece::None) {
            return false;
        }
        file += fileStep;
        rank += rankStep;
    }

    return true;
}
    // This version validates pawns and knights only.
    if (movingPiece != Piece::WhitePawn && movingPiece != Piece::BlackPawn) {
        return false;
    }

    const int direction = (colorOf(movingPiece) == Color::White) ? 8 : -8;
    const int fileDifference = (to % 8) - (from % 8);

    // One square forward into an empty square.
    if (to == from + direction && targetPiece == Piece::None) {
        return true;
    }

    // Two squares forward from the starting rank, with both squares empty.
    const int startingRank = (colorOf(movingPiece) == Color::White) ? 1 : 6;
    if (from / 8 == startingRank &&
        to == from + 2 * direction &&
        targetPiece == Piece::None &&
        pieceAt(from + direction) == Piece::None) {
        return true;
    }

    // One square diagonally forward to capture an opposing piece.
    if (to == from + direction + fileDifference &&
        (fileDifference == -1 || fileDifference == 1) &&
        targetPiece != Piece::None) {
        return true;
    }

    return false;
}

void Board::movePiece(int from, int to) {
    if (!isPseudoLegalMove(from, to)) {
        return;
    }

    const Piece movingPiece = pieceAt(from);
    const Piece targetPiece = pieceAt(to);

    if (targetPiece != Piece::None) {
        removePiece(targetPiece, to);
    }

    removePiece(movingPiece, from);
    putPiece(movingPiece, to);

    sideToMove = (sideToMove == Color::White) ? Color::Black : Color::White;
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
            const Piece piece = pieceAt(square);
            const char shown = pieceCharacter(piece);
            std::cout << shown << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h\n";
        
    }

    
