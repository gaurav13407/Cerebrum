#include "board.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <bit>

namespace {
constexpr std::uint64_t bit(int square) { return std::uint64_t{1} << square; }

constexpr int pieceIndex(Piece piece) { return static_cast<int>(piece); }

constexpr Color colorOf(Piece piece) {
  return pieceIndex(piece) < 6 ? Color::White : Color::Black;
}
bool isPromotionPiece(Piece piece, Color color) {
    if(color==Color::White){
        return piece==Piece::WhiteQueen||
            piece==Piece::WhiteRook||
            piece==Piece::WhiteBishop||
            piece==Piece::WhiteKnight;
    }
    return piece==Piece::BlackQueen||
        piece==Piece::BlackRook||
        piece==Piece::BlackBishop||
        piece==Piece::BlackKnight;
}
char pieceCharacter(Piece piece) {
  switch (piece) {
  case Piece::WhitePawn:
    return 'P';
  case Piece::WhiteKnight:
    return 'N';
  case Piece::WhiteBishop:
    return 'B';
  case Piece::WhiteRook:
    return 'R';
  case Piece::WhiteQueen:
    return 'Q';
  case Piece::WhiteKing:
    return 'K';
  case Piece::BlackPawn:
    return 'p';
  case Piece::BlackKnight:
    return 'n';
  case Piece::BlackBishop:
    return 'b';
  case Piece::BlackRook:
    return 'r';
  case Piece::BlackQueen:
    return 'q';
  case Piece::BlackKing:
    return 'k';
  case Piece::None:
    return '.';
  }
  return '.';
}
} // namespace

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

//To check which color of king is under attack 
 
bool Board::isInCheck(Color color) const{
    const Piece king=
        (color == Color::White) ? Piece::WhiteKing : Piece::BlackKing;
    const std::uint64_t kingBitboard=pieces[static_cast<int>(king)];

    //Temproary test position may not conatin key 
    if(kingBitboard==0){
        return false;
    }
    const int kingSquare=std::countr_zero(kingBitboard);
    const Color attacker=
        (color==Color::White)?Color::Black : Color::White;

    return isSquareAttacked(kingSquare,attacker);
}


//legal move in which it reject move that put king under attack 
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

    // A king is checked, never captured.
    if (targetPiece == Piece::WhiteKing || targetPiece == Piece::BlackKing) {
        return false;
    }
    // Try the move on a copy so the real board stays untouched.
    Board next = *this;
    if (targetPiece != Piece::None) {
        next.removePiece(targetPiece, to);
    }
    next.removePiece(movingPiece, from);
    next.putPiece(promotes ? promotion : movingPiece, to);

    return !next.isInCheck(colorOf(movingPiece));
}

bool Board::isSquareAttacked(int square, Color byColor) const {
    if (square < 0 || square >= 64) {
        return false;
    }

    const int targetFile = square % 8;
    const int targetRank = square / 8;

    // Pawns attack diagonally, even when the target square is empty.
    const Piece attackingPawn =
        (byColor == Color::White) ? Piece::WhitePawn : Piece::BlackPawn;
    const int pawnSourceRank =
        targetRank + ((byColor == Color::White) ? -1 : 1);

    if (pawnSourceRank >= 0 && pawnSourceRank < 8) {
        constexpr int pawnFileOffsets[2] = {-1, 1};
        for (int fileOffset : pawnFileOffsets) {
            const int file = targetFile + fileOffset;
            if (file >= 0 && file < 8 &&
                pieceAt(pawnSourceRank * 8 + file) == attackingPawn) {
                return true;
            }
        }
    }

    // Knights attack from any of these eight source squares.
    const Piece attackingKnight =
        (byColor == Color::White) ? Piece::WhiteKnight : Piece::BlackKnight;
    constexpr int knightOffsets[8][2] = {
        {1, 2}, {2, 1}, {2, -1}, {1, -2},
        {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
    };

    for (const auto& offset : knightOffsets) {
        const int file = targetFile + offset[0];
        const int rank = targetRank + offset[1];
        if (file >= 0 && file < 8 && rank >= 0 && rank < 8 &&
            pieceAt(rank * 8 + file) == attackingKnight) {
            return true;
        }
    }

    // An opposing king attacks each adjacent square.
    const Piece attackingKing =
        (byColor == Color::White) ? Piece::WhiteKing : Piece::BlackKing;
    for (int rankOffset = -1; rankOffset <= 1; ++rankOffset) {
        for (int fileOffset = -1; fileOffset <= 1; ++fileOffset) {
            if (fileOffset == 0 && rankOffset == 0) {
                continue;
            }

            const int file = targetFile + fileOffset;
            const int rank = targetRank + rankOffset;
            if (file >= 0 && file < 8 && rank >= 0 && rank < 8 &&
                pieceAt(rank * 8 + file) == attackingKing) {
                return true;
            }
        }
    }

    // Scan outward from the target; the first piece on a ray blocks anything
    // behind it and attacks only if it is the appropriate opposing slider.
    constexpr int orthogonalDirections[4][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };
    for (const auto& direction : orthogonalDirections) {
        int file = targetFile + direction[0];
        int rank = targetRank + direction[1];

        while (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
            const Piece piece = pieceAt(rank * 8 + file);
            if (piece != Piece::None) {
                const bool isRookOrQueen =
                    piece == Piece::WhiteRook || piece == Piece::BlackRook ||
                    piece == Piece::WhiteQueen || piece == Piece::BlackQueen;
                if (colorOf(piece) == byColor && isRookOrQueen) {
                    return true;
                }
                break;
            }
            file += direction[0];
            rank += direction[1];
        }
    }

    constexpr int diagonalDirections[4][2] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for (const auto& direction : diagonalDirections) {
        int file = targetFile + direction[0];
        int rank = targetRank + direction[1];

        while (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
            const Piece piece = pieceAt(rank * 8 + file);
            if (piece != Piece::None) {
                const bool isBishopOrQueen =
                    piece == Piece::WhiteBishop || piece == Piece::BlackBishop ||
                    piece == Piece::WhiteQueen || piece == Piece::BlackQueen;
                if (colorOf(piece) == byColor && isBishopOrQueen) {
                    return true;
                }
                break;
            }
            file += direction[0];
            rank += direction[1];
        }
    }

    return false;
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

  // King move validation
  if (movingPiece == Piece::WhiteKing || movingPiece == Piece::BlackKing) {
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
      (fileDifference == -1 || fileDifference == 1) &&
      targetPiece != Piece::None) {
    return true;
  }

  return false;
}

void Board::movePiece(int from, int to, Piece promotion) {
  if (!isLegalMove(from, to, promotion)) {
    return;
  }

  const Piece movingPiece = pieceAt(from);
  const Piece targetPiece = pieceAt(to);

  if (targetPiece != Piece::None) {
    removePiece(targetPiece, to);
  }

  removePiece(movingPiece, from);

  const bool promotes =
      (movingPiece == Piece::WhitePawn && to / 8 == 7) ||
      (movingPiece == Piece::BlackPawn && to / 8 == 0);
  putPiece(promotes ? promotion : movingPiece, to);

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
  putPiece(Piece::WhiteRook, 0);
  putPiece(Piece::WhiteKnight, 1);
  putPiece(Piece::WhiteBishop, 2);
  putPiece(Piece::WhiteQueen, 3);
  putPiece(Piece::WhiteKing, 4);
  putPiece(Piece::WhiteBishop, 5);
  putPiece(Piece::WhiteKnight, 6);
  putPiece(Piece::WhiteRook, 7);

  // White pawns: a2 through h2.
  for (int square = 8; square < 16; ++square) {
    putPiece(Piece::WhitePawn, square);
  }

  // Black pawns: a7 through h7.
  for (int square = 48; square < 56; ++square) {
    putPiece(Piece::BlackPawn, square);
  }

  // Black back rank: a8 through h8.
  putPiece(Piece::BlackRook, 56);
  putPiece(Piece::BlackKnight, 57);
  putPiece(Piece::BlackBishop, 58);
  putPiece(Piece::BlackQueen, 59);
  putPiece(Piece::BlackKing, 60);
  putPiece(Piece::BlackBishop, 61);
  putPiece(Piece::BlackKnight, 62);
  putPiece(Piece::BlackRook, 63);
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
