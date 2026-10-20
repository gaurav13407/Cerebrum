#include "search.h"
#include "eval.h"

#include <vector>

namespace {
constexpr int MateScore = 30000;
constexpr int Infinity = 32000;

int negamax(Board& board, int depth, int alpha, int beta,
            int ply, std::uint64_t& nodes) {
  ++nodes;

  const std::vector<Move> moves = board.generateLegalMoves();
  if (moves.empty()) {
    return board.isInCheck(board.sideToMove) ? -MateScore + ply : 0;
  }
  if (depth == 0) {
    return evaluateMaterial(board);
  }

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

  const std::vector<Move> moves = board.generateLegalMoves();
  if (moves.empty()) {
    result.score = board.isInCheck(board.sideToMove) ? -MateScore : 0;
    return result;
  }

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
