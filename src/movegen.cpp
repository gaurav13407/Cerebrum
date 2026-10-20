#include "board.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr Color colorOf(Piece piece) {
  return static_cast<int>(piece) < 6 ? Color::White : Color::Black;
}

bool isPromotionPiece(Piece piece, Color color) {
  if (color == Color::White) {
    return piece == Piece::WhiteQueen || piece == Piece::WhiteRook ||
           piece == Piece::WhiteBishop || piece == Piece::WhiteKnight;
  }
  return piece == Piece::BlackQueen || piece == Piece::BlackRook ||
         piece == Piece::BlackBishop || piece == Piece::BlackKnight;
}
} // namespace

bool Board::isLegalMove(int from, int to, Piece promotion) const {
    if (!isPseudoLegalMove(from, to)) {
        return false;
    }
    const Piece movingPiece = pieceAt(from);
    const Piece targetPiece = pieceAt(to);

    const bool promotes =
        (movingPiece == Piece::WhitePawn && to / 8 == 7) ||
        (movingPiece == Piece::BlackPawn && to / 8 == 0);
    if (promotes) {
        if (!isPromotionPiece(promotion, colorOf(movingPiece))) {
            return false;
        }
    } else if (promotion != Piece::None) {
        return false;
    }

    const bool isCastling =
        (movingPiece == Piece::WhiteKing || movingPiece == Piece::BlackKing) &&
        std::abs(to - from) == 2;

    // A king is checked, never captured.
    if (targetPiece == Piece::WhiteKing || targetPiece == Piece::BlackKing) {
        return false;
    }

    if (isCastling) {
        const Color kingColor = colorOf(movingPiece);
        const Color enemyColor =
            (kingColor == Color::White) ? Color::Black : Color::White;
        if (isInCheck(kingColor)) {
            return false;
        }

        // Test the transit square after the king has vacated its start square.
        const int transitSquare = from + ((to > from) ? 1 : -1);
        Board transitPosition = *this;
        transitPosition.removePiece(movingPiece, from);
        transitPosition.putPiece(movingPiece, transitSquare);
        if (transitPosition.isSquareAttacked(transitSquare, enemyColor)) {
            return false;
        }
    }

    // Try the move on a copy so the real board stays untouched.
    Board next = *this;
    if (targetPiece != Piece::None) {
        next.removePiece(targetPiece, to);
    }
    const int fileDifference = (to % 8) - (from % 8);
    const bool enPassantCapture =
        (movingPiece == Piece::WhitePawn || movingPiece == Piece::BlackPawn) &&
        targetPiece == Piece::None && to == enPassantSquare &&
        (fileDifference == -1 || fileDifference == 1);
    if (enPassantCapture) {
        const int direction =
            (colorOf(movingPiece) == Color::White) ? 8 : -8;
        const int capturedSquare = to - direction;
        const Piece capturedPawn = pieceAt(capturedSquare);
        if (capturedPawn != Piece::None) {
            next.removePiece(capturedPawn, capturedSquare);
        }
    }
    next.removePiece(movingPiece, from);
    next.putPiece(promotes ? promotion : movingPiece, to);

    if (isCastling) {
        const int rookFrom = (to > from) ? from + 3 : from - 4;
        const int rookTo = (to > from) ? from + 1 : from - 1;
        const Piece rook = pieceAt(rookFrom);
        next.removePiece(rook, rookFrom);
        next.putPiece(rook, rookTo);
    }

    return !next.isInCheck(colorOf(movingPiece));
}

std::vector<Move> Board::generateLegalMoves() const {
    std::vector<Move> moves;
    constexpr Piece promotions[] = {
        Piece::WhiteQueen, Piece::WhiteRook, Piece::WhiteBishop, Piece::WhiteKnight,
        Piece::BlackQueen, Piece::BlackRook, Piece::BlackBishop, Piece::BlackKnight};

    for (int from = 0; from < 64; ++from) {
        const Piece movingPiece = pieceAt(from);
        if (movingPiece == Piece::None || colorOf(movingPiece) != sideToMove) {
            continue;
        }
        for (int to = 0; to < 64; ++to) {
            const bool promotes =
                (movingPiece == Piece::WhitePawn && to / 8 == 7) ||
                (movingPiece == Piece::BlackPawn && to / 8 == 0);
            if (promotes) {
                for (Piece promotion : promotions) {
                    if (isLegalMove(from, to, promotion)) {
                        moves.push_back({from, to, promotion});
                    }
                }
            } else if (isLegalMove(from, to)) {
                moves.push_back({from, to, Piece::None});
            }
        }
    }
    return moves;
}

std::uint64_t Board::perft(int depth) {
    if (depth < 0) {
        return 0;
    }
    if (depth == 0) {
        return 1;
    }
    const std::vector<Move> moves = generateLegalMoves();
    if (depth == 1) {
        return moves.size();
    }
    std::uint64_t nodes = 0;
    for (const Move& move : moves) {
        UndoInfo undo;

        if(!makeMove(move,undo)){
            continue;
        }
        nodes+=perft(depth-1);
        unmakeMove(undo);
        
    }
    return nodes;
}

// Node count for each legal move from a position
void Board::perftDivide(int depth) const{
    if(depth <1){
        return;
    }
    const std::vector<Move> moves=generateLegalMoves();
    std::uint64_t totalNodes=0;

    for(const Move &move:moves){
        Board child=*this;
        child.movePiece(move.from, move.to, move.promotion);

        const std::uint64_t nodes=child.perft(depth-1);
        totalNodes+=nodes;

        const auto squareName=[](int square){
            std::string name;
            name += static_cast<char>('a' + square % 8);
            name += static_cast<char>('1' + square / 8);
            return name;
        };
        std::cout << squareName(move.from) << squareName(move.to);

        switch(move.promotion){
            case Piece::WhiteQueen:
            case Piece::BlackQueen:
                std::cout << 'q';
                break;
            case Piece::WhiteRook:
            case Piece::BlackRook:
                std::cout << 'r';
                break;
            case Piece::WhiteBishop:
            case Piece::BlackBishop:
                std::cout << 'b';
                break;
            case Piece::WhiteKnight:
            case Piece::BlackKnight:
                std::cout << 'n';
                break;
            default:
                break;
        }
        std::cout << ": " << nodes << '\n';
    }
    std::cout << "Total: " << totalNodes << '\n';
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
  if (movingPiece == Piece::WhiteKnight || movingPiece == Piece::BlackKnight) {
    const int fileDistance = std::abs((to % 8) - (from % 8));
    const int rankDistance = std::abs((to / 8) - (from / 8));

    return (fileDistance == 1 && rankDistance == 2) ||
           (fileDistance == 2 && rankDistance == 1);
  }

  // Bishop move validation
  if (movingPiece == Piece::WhiteBishop || movingPiece == Piece::BlackBishop) {
    const int fromFile = from % 8;
    const int fromRank = from / 8;
    const int toFile = to % 8;
    const int toRank = to / 8;

    const int fileDistance = std::abs(toFile - fromFile);
    const int rankDistance = std::abs(toRank - fromRank);
    if (fileDistance == 0 || fileDistance != rankDistance) {
      return false;
    }

    const int fileStep = (toFile > fromFile) ? 1 : -1;
    const int rankStep = (toRank > fromRank) ? 1 : -1;
    int file = fileStep + fromFile;
    int rank = rankStep + fromRank;

    while (file != toFile && rank != toRank) {
      if (pieceAt(rank * 8 + file) != Piece::None) {
        return false;
      }
      file += fileStep;
      rank += rankStep;
    }
    return true;
  }
  // Rooks move validation
  if (movingPiece == Piece::WhiteRook || movingPiece == Piece::BlackRook) {
    const int fromFile = from % 8;
    const int fromRank = from / 8;
    const int toFile = to % 8;
    const int toRank = to / 8;

    // A rook must move along the same file or rank.
    if (fromFile != toFile && fromRank != toRank) {
      return false;
    }

    const int fileStep = (toFile > fromFile) ? 1 : (toFile < fromFile ? -1 : 0);
    const int rankStep = (toRank > fromRank) ? 1 : (toRank < fromRank ? -1 : 0);

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
  // Queen Validate Move
  if (movingPiece == Piece::WhiteQueen || movingPiece == Piece::BlackQueen) {
    const int fromFile = from % 8;
    const int fromRank = from / 8;
    const int toFile = to % 8;
    const int toRank = to / 8;

    const int fileDistance = std::abs(toFile - fromFile);
    const int rankDistance = std::abs(toRank - fromRank);

    const bool straight = (fromFile == toFile || fromRank == toRank);
    const bool diagonal = (fileDistance == rankDistance);

    if (!straight && !diagonal) {
      return false;
    }

    const int fileStep = (toFile > fromFile) ? 1 : (toFile < fromFile ? -1 : 0);
    const int rankStep = (toRank > fromRank) ? 1 : (toRank < fromRank ? -1 : 0);

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

  // King moves and castling.
  if (movingPiece == Piece::WhiteKing || movingPiece == Piece::BlackKing) {
    if (movingPiece == Piece::WhiteKing && from == 4) {
      if (to == 6 && (castlingRights & WhiteKingSide) != 0 &&
          pieceAt(7) == Piece::WhiteRook && pieceAt(5) == Piece::None &&
          pieceAt(6) == Piece::None) {
        return true;
      }
      if (to == 2 && (castlingRights & WhiteQueenSide) != 0 &&
          pieceAt(0) == Piece::WhiteRook && pieceAt(1) == Piece::None &&
          pieceAt(2) == Piece::None && pieceAt(3) == Piece::None) {
        return true;
      }
    }

    if (movingPiece == Piece::BlackKing && from == 60) {
      if (to == 62 && (castlingRights & BlackKingSide) != 0 &&
          pieceAt(63) == Piece::BlackRook && pieceAt(61) == Piece::None &&
          pieceAt(62) == Piece::None) {
        return true;
      }
      if (to == 58 && (castlingRights & BlackQueenSide) != 0 &&
          pieceAt(56) == Piece::BlackRook && pieceAt(57) == Piece::None &&
          pieceAt(58) == Piece::None && pieceAt(59) == Piece::None) {
        return true;
      }
    }

    const int fileDistance = std::abs((to % 8) - (from % 8));
    const int rankDistance = std::abs((to / 8) - (from / 8));

    return fileDistance <= 1 && rankDistance <= 1;
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
  if (from / 8 == startingRank && to == from + 2 * direction &&
      targetPiece == Piece::None && pieceAt(from + direction) == Piece::None) {
    return true;
  }

  // One square diagonally forward to capture an opposing piece.
  if (to == from + direction + fileDifference &&
      (fileDifference == -1 || fileDifference == 1)) {
    if (targetPiece != Piece::None) {
      return true;
    }

    // En passant: the captured pawn is beside the destination square.
    if (to == enPassantSquare) {
      const int capturedSquare = to - direction;
      const Piece expectedPawn =
          (colorOf(movingPiece) == Color::White)
              ? Piece::BlackPawn
              : Piece::WhitePawn;
      return pieceAt(capturedSquare) == expectedPawn;
    }
  }

  return false;
}

