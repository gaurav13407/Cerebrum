# Move generation: `movegen.cpp`

Move generation provides the legal moves search and the UCI interface need.
It first checks how a piece moves, then filters moves that leave its own king
in check. Board storage and attack detection are described in `board.md`.

- `isPromotionPiece(piece, color)` (file-local helper) accepts only a queen,
  rook, bishop, or knight belonging to the promoting side.
- `Board::isPseudoLegalMove(from, to)` checks piece movement rules and clear
  paths, including pawn movement, en passant, and castling availability. It
  does not by itself ensure the king is safe.
- `Board::isLegalMove(from, to, promotion)` validates promotion and special
  move rules, simulates the move on a copy, and rejects it if the moving side's
  king would be in check. For castling it also checks the king's start and
  transit squares.
- `Board::generateLegalMoves()` checks each piece and destination for the side
  to move and returns all legal moves, including all four promotion choices.
- `Board::perft(depth)` recursively counts the legal move paths to `depth`.
  Depth zero returns one. This is mainly used to check move generation.
- `Board::perftDivide(depth)` prints each legal first move and its subtree
  count, followed by the total. It helps locate move-generation errors.

`perftDivide()` uses a small local square-name conversion to print coordinates;
it has no separate public helper.
