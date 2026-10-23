#include "uci.h"

#include "board.h"
#include "search.h"
#include "tt.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
Piece promotionPiece(char symbol, Color sideToMove) {
  switch (static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)))) {
  case 'q': return sideToMove == Color::White ? Piece::WhiteQueen : Piece::BlackQueen;
  case 'r': return sideToMove == Color::White ? Piece::WhiteRook : Piece::BlackRook;
  case 'b': return sideToMove == Color::White ? Piece::WhiteBishop : Piece::BlackBishop;
  case 'n': return sideToMove == Color::White ? Piece::WhiteKnight : Piece::BlackKnight;
  default: return Piece::None;
  }
}

bool parseUciMove(const std::string& text, Board& board, Move& parsed) {
  if (text.size() != 4 && text.size() != 5)
    return false;
  if (text[0] < 'a' || text[0] > 'h' || text[2] < 'a' || text[2] > 'h' ||
      text[1] < '1' || text[1] > '8' || text[3] < '1' || text[3] > '8')
    return false;

  const int from = (text[1] - '1') * 8 + text[0] - 'a';
  const int to = (text[3] - '1') * 8 + text[2] - 'a';
  const Piece promotion = text.size() == 5
      ? promotionPiece(text[4], board.sideToMove) : Piece::None;
  if (text.size() == 5 && promotion == Piece::None)
    return false;

  for (const Move& move : board.generateLegalMoves()) {
    if (move.from == from && move.to == to && move.promotion == promotion) {
      parsed = move;
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

bool setPosition(const std::vector<std::string>& tokens, Board& board,
                 std::vector<std::uint64_t>& history) {
  if (tokens.size() < 2)
    return false;

  Board nextBoard;
  std::size_t index = 1;
  if (tokens[index] == "startpos") {
    nextBoard.setStartingPosition();
    ++index;
  } else if (tokens[index] == "fen") {
    ++index;
    const std::size_t fenStart = index;
    while (index < tokens.size() && tokens[index] != "moves")
      ++index;
    if (index - fenStart != 6)
      return false;
    std::string fen;
    for (std::size_t i = fenStart; i < index; ++i) {
      if (!fen.empty()) fen += ' ';
      fen += tokens[i];
    }
    if (!nextBoard.loadFEN(fen))
      return false;
  } else {
    return false;
  }

  std::vector<std::uint64_t> nextHistory{zobristHash(nextBoard)};
  if (index < tokens.size()) {
    if (tokens[index] != "moves")
      return false;
    ++index;
    for (; index < tokens.size(); ++index) {
      Move move;
      if (!parseUciMove(tokens[index], nextBoard, move))
        return false;
      UndoInfo undo;
      if (!nextBoard.makeMove(move, undo))
        return false;
      nextHistory.push_back(zobristHash(nextBoard));
    }
  }

  board = nextBoard;
  history = std::move(nextHistory);
  return true;
}

int parsePositive(const std::string& value, int fallback) {
  try {
    std::size_t used = 0;
    const int parsed = std::stoi(value, &used);
    return used == value.size() && parsed > 0 ? parsed : fallback;
  } catch (...) {
    return fallback;
  }
}

void searchAndReport(Board& board, const std::vector<std::uint64_t>& history,
                     const std::vector<std::string>& tokens) {
  int depth = 64;
  int timeMs = 5000;
  int whiteTime = -1, blackTime = -1, whiteInc = 0, blackInc = 0;
  bool hasMoveTime = false;

  for (std::size_t i = 1; i + 1 < tokens.size(); ++i) {
    if (tokens[i] == "depth") depth = parsePositive(tokens[++i], depth);
    else if (tokens[i] == "movetime") {
      timeMs = parsePositive(tokens[++i], timeMs);
      hasMoveTime = true;
    } else if (tokens[i] == "wtime") whiteTime = parsePositive(tokens[++i], whiteTime);
    else if (tokens[i] == "btime") blackTime = parsePositive(tokens[++i], blackTime);
    else if (tokens[i] == "winc") whiteInc = std::max(0, parsePositive(tokens[++i], 0));
    else if (tokens[i] == "binc") blackInc = std::max(0, parsePositive(tokens[++i], 0));
  }

  if (!hasMoveTime) {
    const int remaining = board.sideToMove == Color::White ? whiteTime : blackTime;
    const int increment = board.sideToMove == Color::White ? whiteInc : blackInc;
    if (remaining > 0)
      timeMs = std::max(10, std::min(remaining / 30 + increment * 3 / 4,
                                     std::max(10, remaining - 20)));
  }
  depth = std::clamp(depth, 1, 64);
  timeMs = std::clamp(timeMs, 1, 600000);

  SearchResult result = findBestMove(board, depth, timeMs, history);
  const int score = result.score;
  std::cout << "info depth " << result.completedDepth << " score ";
  if (score > 29000)
    std::cout << "mate " << std::max(1, (30000 - score + 1) / 2);
  else if (score < -29000)
    std::cout << "mate " << -std::max(1, (30000 + score + 1) / 2);
  else
    std::cout << "cp " << score;
  std::cout << " nodes " << result.nodes << "\n";
  std::cout << "bestmove " << (result.hasMove ? moveText(result.bestMove) : "0000")
            << std::endl;
}
} // namespace

int runUci() {
  Board board;
  board.setStartingPosition();
  std::vector<std::uint64_t> history{zobristHash(board)};
  std::string line;

  while (std::getline(std::cin, line)) {
    std::istringstream input(line);
    std::vector<std::string> tokens;
    for (std::string token; input >> token;)
      tokens.push_back(token);
    if (tokens.empty()) continue;

    if (tokens[0] == "uci") {
      std::cout << "id name Cerebrum\nid author Gaurav\nuciok" << std::endl;
    } else if (tokens[0] == "isready") {
      std::cout << "readyok" << std::endl;
    } else if (tokens[0] == "ucinewgame") {
      board.setStartingPosition();
      history.assign(1, zobristHash(board));
    } else if (tokens[0] == "position") {
      if (!setPosition(tokens, board, history))
        std::cerr << "info string invalid position command" << std::endl;
    } else if (tokens[0] == "go") {
      searchAndReport(board, history, tokens);
    } else if (tokens[0] == "quit") {
      break;
    } else if (tokens[0] == "stop") {
      // Search is time bounded and runs synchronously; it returns bestmove itself.
    }
  }
  return 0;
}
