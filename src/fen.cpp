#include "board.h"

#include <sstream>
#include <string>

bool Board::loadFEN(const std::string &fen){
    std::istringstream input(fen);
    std::string placement;
    std::string activeColor;
    std::string castling;
    std::string enPassant;
    std::string halfmoveClock;
    std::string fullmoveNumber;

    if(!(input >> placement >> activeColor >> castling >> enPassant >> halfmoveClock >> fullmoveNumber)){
        return false;
    }
    Board parsed;
    parsed.clear();

    try {
        std::size_t charactersRead = 0;
        parsed.halfmoveClock = std::stoi(halfmoveClock, &charactersRead);
        if (charactersRead != halfmoveClock.size() || parsed.halfmoveClock < 0) {
            return false;
        }
    } catch (...) {
        return false;
    }

    int rank=7;
    int file=0;

    for(char c:placement){
        if(c=='/'){
            if (file != 8 || rank == 0) return false;
            --rank;
            file=0;
            continue;
        }
        if (c >= '1' && c <= '8') {
            file+=c-'0';
            if(file>8) return false;
            continue;
        }
        Piece piece;
        switch (c) {
        case 'P': piece = Piece::WhitePawn; break;
        case 'N': piece = Piece::WhiteKnight; break;
        case 'B': piece = Piece::WhiteBishop; break;
        case 'R': piece = Piece::WhiteRook; break;
        case 'Q': piece = Piece::WhiteQueen; break;
        case 'K': piece = Piece::WhiteKing; break;
        case 'p': piece = Piece::BlackPawn; break;
        case 'n': piece = Piece::BlackKnight; break;
        case 'b': piece = Piece::BlackBishop; break;
        case 'r': piece = Piece::BlackRook; break;
        case 'q': piece = Piece::BlackQueen; break;
        case 'k': piece = Piece::BlackKing; break;
        default: return false;
        }
        if (file >= 8 || rank < 0) return false;
        parsed.putPiece(piece,rank*8+file);
        ++file;
    }
    if (rank != 0 || file != 8) return false;
    if(activeColor=="w"){
        parsed.sideToMove=Color::White;
    }else if(activeColor=="b"){
        parsed.sideToMove=Color::Black;
    }
    else{
        return false;
    }
    parsed.castlingRights = 0;
    if(castling!="-"){
        for(char c:castling){
        switch (c) {
            case 'K': parsed.castlingRights |= WhiteKingSide; break;
            case 'Q': parsed.castlingRights |= WhiteQueenSide; break;
            case 'k': parsed.castlingRights |= BlackKingSide; break;
            case 'q': parsed.castlingRights |= BlackQueenSide; break;
            default: return false;
            }
        }
    }
     
parsed.enPassantSquare = -1;
    if (enPassant != "-") {
        if (enPassant.size() != 2 ||
            enPassant[0] < 'a' || enPassant[0] > 'h' ||
            enPassant[1] < '1' || enPassant[1] > '8') {
            return false;
        }
        parsed.enPassantSquare =
            (enPassant[1] - '1') * 8 + (enPassant[0] - 'a');
    }

    *this = parsed;
    return true;

}

