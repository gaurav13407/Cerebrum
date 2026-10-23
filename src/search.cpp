#include "search.h"
#include "eval.h"
#include "tt.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
#include <chrono>

namespace {
constexpr int MateScore = 30000;
constexpr int Infinity = 32000;

struct SearchControl{
    std::chrono::steady_clock::time_point deadline;
    bool stopped=false;

    bool shouldStop(std::uint64_t nodes){
        if(stopped){
            return true;
        }
        //Check clock every 1024 nodes to avoide checking it on every node.
        if((nodes& 1023u)==0 &&
                std::chrono::steady_clock::now() >=deadline){
            stopped =true;
        }
        return stopped;
    }
};
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

bool isCaptureMove(const Board &board, const  Move& move){
    if(board.pieceAt(move.to)!=Piece::None){
        return true;
    }
    const Piece movingPiece=board.pieceAt(move.from);
    const bool isPawn=
        movingPiece==Piece::WhitePawn||
        movingPiece==Piece::BlackPawn;

    return isPawn && 
        move.to==board.enPassantSquare &&
        move.from %8 !=move.to %8;
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

int quiescence(Board &board, int alpha,int beta, int ply,
               std::uint64_t& nodes, SearchControl& control){
    ++nodes;
    if (control.shouldStop(nodes))
        return 0;
    const bool inCheck=board.isInCheck(board.sideToMove);
    std::vector<Move> moves = board.generateLegalMoves();

    if (moves.empty()) {
        return inCheck ? -MateScore + ply : 0;
    }
    int bestScore=-Infinity;

    if(!inCheck){
        const int standPat=evaluateMaterial(board);

        if(standPat>=beta){
            return standPat;
        }
        if(standPat>alpha){
            alpha=standPat;
        }
        bestScore=standPat;
        moves.erase(
                std::remove_if(moves.begin(), moves.end(),
                    [&board](const Move &move){
                    return !isCaptureMove(board,move) &&
                    move.promotion==Piece::None;
                    }
                ),
                moves.end());
    }
    orderMoves(board,moves);

    for(const Move &move :moves){
        UndoInfo undo;
        if(!board.makeMove(move,undo)){
            continue;
        }
        const int score=
            -quiescence(board, -beta, -alpha, ply + 1, nodes, control);

        board.unmakeMove(undo);
        if (control.stopped)
            return 0;

        if(score>bestScore){
            bestScore=score;
        }
        if(score>alpha){
            alpha=score;
        }
        if(alpha>=beta){
            break;
        }
    }
    return bestScore;
}
int negamax(Board& board, int depth, int alpha, int beta,
            int ply, std::uint64_t& nodes, std::uint64_t& transpositionHits,
            TranspositionTable& table, SearchControl& control) {
  ++nodes;
  if (control.shouldStop(nodes))
    return 0;

  const int originalAlpha = alpha;
  const int originalBeta = beta;
  const std::uint64_t key = zobristHash(board);
  TTEntry cached;
  Move tableMove;
  bool hasTableMove = false;

  if (table.probe(key, cached)) {
    ++transpositionHits;
    tableMove = cached.bestMove;
    hasTableMove = true;

    if (cached.depth >= depth) {
        const int cachedScore=scoreFromTable(cached.score, ply);
      if (cached.bound == TTBound::Exact ||
          (cached.bound == TTBound::LowerBound && cachedScore >= beta) ||
          (cached.bound == TTBound::UpperBound && cachedScore <= alpha)) {
        return cachedScore;
      }
    }
  }

  std::vector<Move> moves = board.generateLegalMoves();
  if (moves.empty()) {
    return board.isInCheck(board.sideToMove) ? -MateScore + ply : 0;
  }
  if (depth == 0) {
      return quiescence(board, alpha, beta, ply, nodes, control);
  }
  orderMoves(board, moves);
  if (hasTableMove) {
    const auto cachedMove = std::find_if(
        moves.begin(), moves.end(), [&tableMove](const Move& move) {
          return move.from == tableMove.from && move.to == tableMove.to &&
                 move.promotion == tableMove.promotion;
        });
    if (cachedMove != moves.end())
      std::rotate(moves.begin(), cachedMove, cachedMove + 1);
  }

  int bestScore = -Infinity;
  Move bestMove;
  for (const Move& move : moves) {
    UndoInfo undo;
    if (!board.makeMove(move, undo)) {
      continue;
    }

    const int score =
        -negamax(board, depth - 1, -beta, -alpha, ply + 1, nodes,
                 transpositionHits, table, control);
    board.unmakeMove(undo);
    if (control.stopped)
      return 0;

    if (score > bestScore) {
      bestScore = score;
      bestMove = move;
    }
    if (score > alpha) {
      alpha = score;
    }
    if (alpha >= beta) {
      break;
    }
  }

  TTEntry entry;
  entry.depth = depth;
  entry.score = scoreToTable(bestScore,ply);
  entry.bestMove = bestMove;
  entry.bound = bestScore <= originalAlpha ? TTBound::UpperBound
                : bestScore >= originalBeta ? TTBound::LowerBound
                                            : TTBound::Exact;
  table.store(key, entry);

  return bestScore;
}
} // namespace

SearchResult findBestMove(Board& board, int depth, int timeLimitMs) {
  static TranspositionTable table;
  SearchResult result;
  if (depth < 1) {
    depth = 1;
  }
  if (timeLimitMs < 1)
    timeLimitMs = 1;

  SearchControl control;
  control.deadline = std::chrono::steady_clock::now() +
                     std::chrono::milliseconds(timeLimitMs);

  const std::vector<Move> rootMoves = board.generateLegalMoves();
  if (rootMoves.empty()) {
    result.score = board.isInCheck(board.sideToMove) ? -MateScore : 0;
    return result;
  }
  // Return a legal fallback if time expires before depth one completes.
  result.bestMove = rootMoves.front();
  result.hasMove = true;

  Move previousBestMove;
  bool hasPreviousBestMove = false;

  for (int currentDepth = 1; currentDepth <= depth; ++currentDepth) {
    std::vector<Move> moves = rootMoves;
    orderMoves(board, moves);

    if (hasPreviousBestMove) {
      const auto previousMove = std::find_if(
          moves.begin(), moves.end(),
          [&previousBestMove](const Move& move) {
            return move.from == previousBestMove.from &&
                   move.to == previousBestMove.to &&
                   move.promotion == previousBestMove.promotion;
          });
      if (previousMove != moves.end()) {
        std::rotate(moves.begin(), previousMove, previousMove + 1);
      }
    }

    int alpha = -Infinity;
    constexpr int beta = Infinity;
    int bestScore = -Infinity;
    Move iterationBestMove;
    bool foundMove = false;

    for (const Move& move : moves) {
      if (std::chrono::steady_clock::now() >= control.deadline) {
        control.stopped = true;
        break;
      }
      UndoInfo undo;
      if (!board.makeMove(move, undo)) {
        continue;
      }

      const int score =
          -negamax(board, currentDepth - 1, -beta, -alpha, 1, result.nodes,
                   result.transpositionHits, table, control);
      board.unmakeMove(undo);
      if (control.stopped)
        break;

      if (!foundMove || score > bestScore) {
        bestScore = score;
        iterationBestMove = move;
        foundMove = true;
      }
      if (score > alpha) {
        alpha = score;
      }
    }

    // Never publish a score from an iteration that did not finish.
    if (control.stopped)
      break;

    if (!foundMove) {
      break;
    }

    result.bestMove = iterationBestMove;
    result.score = bestScore;
    result.hasMove = true;
    result.completedDepth = currentDepth;
    previousBestMove = iterationBestMove;
    hasPreviousBestMove = true;
  }

  if (!result.hasMove) {
    result.score = evaluateMaterial(board);
  }
  return result;
}
