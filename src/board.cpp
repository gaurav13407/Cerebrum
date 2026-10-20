#include "board.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <bit>
namespace {
constexpr std::uint64_t bit(int square) { return std::uint64_t{1} << square; }

constexpr int pieceIndex(Piece piece) { return static_cast<int>(piece); }

constexpr Color colorOf(Piece piece) {
  return pieceIndex(piece) < 6 ? Color::White : Color::Black;
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

void Board::movePiece(int from, int to, Piece promotion) {
  UndoInfo undo;
  makeMove(Move{from, to, promotion}, undo);
}

bool Board::makeMove(const Move& move, UndoInfo& undo) {
  const int from = move.from;
  const int to = move.to;
  if (!isLegalMove(from, to, move.promotion)) {
    return false;
  }

  const Piece movingPiece = pieceAt(from);
  const Piece targetPiece = pieceAt(to);
  const bool isCastling =
      (movingPiece == Piece::WhiteKing || movingPiece == Piece::BlackKing) &&
      std::abs(to - from) == 2;

  undo.move = move;
  undo.movedPiece = movingPiece;
  undo.capturedPiece = targetPiece;
  undo.capturedSquare = to;
  undo.previousCastlingRights = castlingRights;
  undo.previousEnPassantSquare = enPassantSquare;
  undo.previousSideToMove = sideToMove;

  const int fileDifference = (to % 8) - (from % 8);
  const bool enPassantCapture =
      (movingPiece == Piece::WhitePawn || movingPiece == Piece::BlackPawn) &&
      targetPiece == Piece::None && to == enPassantSquare &&
      (fileDifference == -1 || fileDifference == 1);
  if (enPassantCapture) {
    const int direction = (movingPiece == Piece::WhitePawn) ? 8 : -8;
    undo.capturedSquare = to - direction;
    undo.capturedPiece = pieceAt(undo.capturedSquare);
  }

  if (movingPiece == Piece::WhiteKing) {
    castlingRights &=
        static_cast<std::uint8_t>(~(WhiteKingSide | WhiteQueenSide));
  } else if (movingPiece == Piece::BlackKing) {
    castlingRights &=
        static_cast<std::uint8_t>(~(BlackKingSide | BlackQueenSide));
  }

  if (movingPiece == Piece::WhiteRook && from == 0) {
    castlingRights &= static_cast<std::uint8_t>(~WhiteQueenSide);
  } else if (movingPiece == Piece::WhiteRook && from == 7) {
    castlingRights &= static_cast<std::uint8_t>(~WhiteKingSide);
  } else if (movingPiece == Piece::BlackRook && from == 56) {
    castlingRights &= static_cast<std::uint8_t>(~BlackQueenSide);
  } else if (movingPiece == Piece::BlackRook && from == 63) {
    castlingRights &= static_cast<std::uint8_t>(~BlackKingSide);
  }

  if (targetPiece == Piece::WhiteRook && to == 0) {
    castlingRights &= static_cast<std::uint8_t>(~WhiteQueenSide);
  } else if (targetPiece == Piece::WhiteRook && to == 7) {
    castlingRights &= static_cast<std::uint8_t>(~WhiteKingSide);
  } else if (targetPiece == Piece::BlackRook && to == 56) {
    castlingRights &= static_cast<std::uint8_t>(~BlackQueenSide);
  } else if (targetPiece == Piece::BlackRook && to == 63) {
    castlingRights &= static_cast<std::uint8_t>(~BlackKingSide);
  }

  if (targetPiece != Piece::None) {
    removePiece(targetPiece, to);
  }
  if (enPassantCapture) {
    removePiece(undo.capturedPiece, undo.capturedSquare);
  }

  removePiece(movingPiece, from);
  const bool promotes =
      (movingPiece == Piece::WhitePawn && to / 8 == 7) ||
      (movingPiece == Piece::BlackPawn && to / 8 == 0);
  putPiece(promotes ? move.promotion : movingPiece, to);

  if (isCastling) {
    const int rookFrom = (to > from) ? from + 3 : from - 4;
    const int rookTo = (to > from) ? from + 1 : from - 1;
    const Piece rook = pieceAt(rookFrom);
    removePiece(rook, rookFrom);
    putPiece(rook, rookTo);
  }

  enPassantSquare = -1;
  if ((movingPiece == Piece::WhitePawn || movingPiece == Piece::BlackPawn) &&
      std::abs(to - from) == 16) {
    enPassantSquare = (from + to) / 2;
  }
  sideToMove = (sideToMove == Color::White) ? Color::Black : Color::White;
  return true;
}

void Board::unmakeMove(const UndoInfo& undo) {
  const int from = undo.move.from;
  const int to = undo.move.to;
  const bool wasCastling =
      (undo.movedPiece == Piece::WhiteKing ||
       undo.movedPiece == Piece::BlackKing) &&
      std::abs(to - from) == 2;

  if (wasCastling) {
    const int rookFrom = (to > from) ? from + 3 : from - 4;
    const int rookTo = (to > from) ? from + 1 : from - 1;
    const Piece rook = pieceAt(rookTo);
    removePiece(rook, rookTo);
    putPiece(rook, rookFrom);
  }

  removePiece(pieceAt(to), to);
  putPiece(undo.movedPiece, from);
  if (undo.capturedPiece != Piece::None) {
    putPiece(undo.capturedPiece, undo.capturedSquare);
  }

  castlingRights = undo.previousCastlingRights;
  enPassantSquare = undo.previousEnPassantSquare;
  sideToMove = undo.previousSideToMove;
}

void Board::clear() {
  pieces.fill(0);
  occupancy.fill(0);
  sideToMove = Color::White;
  enPassantSquare = -1;
  castlingRights = 0;
}
//FEN loading to test all chess position

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
  castlingRights = WhiteKingSide | WhiteQueenSide |
                   BlackKingSide | BlackQueenSide;
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
