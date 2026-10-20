#include "eval.h"
#include <bit>

int evaluateMaterial(const Board& board){
    constexpr int pieceValues[6]={
        100, //Pawn 
        320, //Knight 
        330, //Bishop
        500, //Rook
        900, //Queen 
        0, //King
    };

    int whiteScore=0;
    int blackScore=0;

    for(int pieceType=0;pieceType<6;++pieceType){
        const int whiteCount=std::popcount(board.pieces[pieceType]);
        const int blackCount=std::popcount(board.pieces[pieceType+6]);

        whiteScore+=whiteCount*pieceValues[pieceType];
        blackScore+=blackCount*pieceValues[pieceType];
    }
    const int whitePerspectiveScore=whiteScore-blackScore;

    return board.sideToMove == Color::White
        ? whitePerspectiveScore
        : -whitePerspectiveScore;

    }
