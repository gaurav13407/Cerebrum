#include "search.h"
#include "eval.h"
#include <algorithm>
#include <vector>

namespace {
constexpr int MateScore = 30000;
constexpr int Infinity = 32000;

int pieceValue(Piece piece){
    if(piece==Piece::None){
        return 0;
    }
    switch(static_cast<int>(piece)%6){
        case 0:return 100; // pawn
        case 1:return 320; //Knight
        case 2:return 330; //Bishop
        case 3:return 500; //Rook
        case 4:return 900; //Queen
        case 5:return 0; //King
        default: return 0;
    }
}

int moveOrderScore(const Board& board,const Move& move){
    int score=0;
    if(move.promotion!=Piece::None){
        score+=20000+pieceValue(move.promotion);
    }
    Piece capturedPiece=board.pieceAt(move.to);

    //En Passent capture land on an empty square.
    if(capturedPiece==Piece::None &&
        move.to==board.enPassantSquare && 
        move.from %8!=move.to%8){
        capturedPiece=board.sideToMove == Color::White
            ?Piece::BlackPawn
            : Piece::WhitePawn;
    }
    if(capturedPiece!=Piece::None){
        const Piece movingPiece=board.pieceAt(move.from);
        score+=10000+10*pieceValue(capturedPiece)
            -pieceValue(movingPiece);
    }
    return score;
}

void orderMoves(const Board& board,std::vector<Move>&moves){
    std::sort(moves.begin(), moves.end(),
            [&board](const Move& a,const Move&b){
            return moveOrderScore(board, a)>moveOrderScore(board, b);
            });
}

int negamax(Board& board, int depth, int alpha, int beta,
            int ply, std::uint64_t& nodes) {
  ++nodes;

  std::vector<Move> moves = board.generateLegalMoves();
  if (moves.empty()) {
    return board.isInCheck(board.sideToMove) ? -MateScore + ply : 0;
  }
  if (depth == 0) {
    return evaluateMaterial(board);
  }
  orderMoves(board, moves);

  int bestScore = -Infinity;
  for (const Move& move : moves) {
    UndoInfo undo;
    if (!board.makeMove(move, undo)) {
      continue;
    }

    const int score =
        -negamax(board, depth - 1, -beta, -alpha, ply + 1, nodes);
    board.unmakeMove(undo);

    if (score > bestScore) {
      bestScore = score;
    }
    if (score > alpha) {
      alpha = score;
    }
    if (alpha >= beta) {
      break;
    }
  }

  return bestScore;
}
} // namespace

SearchResult findBestMove(Board& board, int depth) {
  SearchResult result;
  if (depth < 1) {
    depth = 1;
  }

  std::vector<Move> moves = board.generateLegalMoves();
  if (moves.empty()) {
    result.score = board.isInCheck(board.sideToMove) ? -MateScore : 0;
    return result;
  }
  orderMoves(board, moves);

  int alpha = -Infinity;
  constexpr int beta = Infinity;
  int bestScore = -Infinity;

  for (const Move& move : moves) {
    UndoInfo undo;
    if (!board.makeMove(move, undo)) {
      continue;
    }

    const int score =
        -negamax(board, depth - 1, -beta, -alpha, 1, result.nodes);
    board.unmakeMove(undo);

    if (!result.hasMove || score > bestScore) {
      bestScore = score;
      result.bestMove = move;
      result.hasMove = true;
    }
    if (score > alpha) {
      alpha = score;
    }
  }

  result.score = result.hasMove ? bestScore : evaluateMaterial(board);
  return result;
}
