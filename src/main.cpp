#include "board.h"
#include "search.h"
#include "tt.h"
#include "uci.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

namespace {
Piece promotionPiece(char symbol, Color sideToMove) {
  switch (symbol) {
  case 'q': return sideToMove == Color::White ? Piece::WhiteQueen : Piece::BlackQueen;
  case 'r': return sideToMove == Color::White ? Piece::WhiteRook : Piece::BlackRook;
  case 'b': return sideToMove == Color::White ? Piece::WhiteBishop : Piece::BlackBishop;
  case 'n': return sideToMove == Color::White ? Piece::WhiteKnight : Piece::BlackKnight;
  default: return Piece::None;
  }
}

bool parseMove(const std::string& input, const std::vector<Move>& legalMoves,
               Color sideToMove, Move& selectedMove) {
  if (input.size() != 4 && input.size() != 5)
    return false;
  if (input[0] < 'a' || input[0] > 'h' || input[2] < 'a' || input[2] > 'h' ||
      input[1] < '1' || input[1] > '8' || input[3] < '1' || input[3] > '8') {
    return false;
  }

  const int from = (input[1] - '1') * 8 + input[0] - 'a';
  const int to = (input[3] - '1') * 8 + input[2] - 'a';
  Piece promotion = Piece::None;
  if (input.size() == 5) {
    promotion = promotionPiece(input[4], sideToMove);
    if (promotion == Piece::None)
      return false;
  }

  for (const Move& move : legalMoves) {
    if (move.from == from && move.to == to && move.promotion == promotion) {
      selectedMove = move;
      return true;
    }
  }
  return false;
}

std::string moveText(const Move& move) {
  std::string text;
  text += static_cast<char>('a' + move.from % 8);
  text += static_cast<char>('1' + move.from / 8);
  text += static_cast<char>('a' + move.to % 8);
  text += static_cast<char>('1' + move.to / 8);
  switch (move.promotion) {
  case Piece::WhiteQueen: case Piece::BlackQueen: text += 'q'; break;
  case Piece::WhiteRook: case Piece::BlackRook: text += 'r'; break;
  case Piece::WhiteBishop: case Piece::BlackBishop: text += 'b'; break;
  case Piece::WhiteKnight: case Piece::BlackKnight: text += 'n'; break;
  default: break;
  }
  return text;
}
} // namespace

int main(int argc, char** argv) {
  if (argc < 2 || std::string(argv[1]) != "--console")
    return runUci();

  constexpr int SearchDepth = 6;
  constexpr int SearchTimeMs = 1000;

  Board board;
  board.setStartingPosition();

  // Keep one hash for the starting position, then append after each real move.
  std::vector<std::uint64_t> gameHistory{zobristHash(board)};

  std::cout << "You are White. Enter moves in coordinate form, like e2e4.\n"
               "For promotion, add q, r, b, or n, like e7e8q. Type quit to exit.\n";

  while (true) {
    board.print();
    const std::vector<Move> legalMoves = board.generateLegalMoves();
    if (legalMoves.empty()) {
      if (board.isInCheck(board.sideToMove))
        std::cout << (board.sideToMove == Color::White ? "Black" : "White")
                  << " wins by checkmate.\n";
      else
        std::cout << "Draw by stalemate.\n";
      break;
    }

    const std::uint64_t currentHash = zobristHash(board);
    if (std::count(gameHistory.begin(), gameHistory.end(), currentHash) >= 3) {
      std::cout << "Draw by threefold repetition.\n";
      break;
    }
    if (board.halfmoveClock >= 100) {
      std::cout << "Draw by the fifty-move rule.\n";
      break;
    }
    if (isInsufficientMaterial(board)) {
      std::cout << "Draw by insufficient material.\n";
      break;
    }

    Move move;
    if (board.sideToMove == Color::White) {
      std::string input;
      std::cout << "Your move: ";
      if (!(std::cin >> input) || input == "quit")
        break;
      std::transform(input.begin(), input.end(), input.begin(),
                     [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

      if (!parseMove(input, legalMoves, board.sideToMove, move)) {
        std::cout << "That move is not legal. Try again.\n";
        continue;
      }
    } else {
      const SearchResult result =
          findBestMove(board, SearchDepth, SearchTimeMs, gameHistory);
      if (!result.hasMove) {
        std::cout << "No legal engine move.\n";
        break;
      }
      move = result.bestMove;
      std::cout << "Engine move: " << moveText(move)
                << " (searched depth " << result.completedDepth << ")\n";
    }

    UndoInfo undo;
    if (!board.makeMove(move, undo)) {
      std::cerr << "Internal error: selected move could not be played.\n";
      return 1;
    }
    gameHistory.push_back(zobristHash(board));
  }

  return 0;
}
