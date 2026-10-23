# Board representation

`Board` stores one chess position. The move generator, FEN loader, evaluation,
and search code all use it to read or update that position.

## `include/board.h`

This is the shared interface: it defines the board data and declares the
operations other files can call.

- `Color` identifies White or Black. `Piece` identifies each colored piece;
  `None` means an empty square.
- `Move` stores a start square, destination square, and optional promotion
  piece.
- `UndoInfo` saves the details that a move changes, so search can restore the
  exact earlier position.
- `Board::pieces` has one 64-bit bitboard for each of the 12 piece types.
  `occupancy` combines the squares occupied by each color. This compact layout
  makes board and move calculations efficient.
- `sideToMove`, `castlingRights`, `enPassantSquare`, and `halfmoveClock` hold
  position details that are not represented by piece locations alone.

### Board operations

- `clear()` empties the board and resets position details.
- `setStartingPosition()` sets up the normal chess starting position.
- `print()` displays the board in the terminal.
- `pieceAt(square)` returns the piece on a square, or `None` if it is empty.
- `putPiece(piece, square)` and `removePiece(piece, square)` update both the
  piece bitboard and that color's occupancy bitboard. Use these helpers to keep
  the board data in sync.
- `movePiece(from, to, promotion)` applies a legal move. For search code that
  needs to undo it, use `makeMove()` and `unmakeMove()` instead.
- `makeMove(move, undo)` checks legality, saves the old state in `undo`, then
  updates pieces and position details. It returns `false` if the move is
  illegal.
- `unmakeMove(undo)` restores the position saved before `makeMove()`.
- `isSquareAttacked(square, color)` checks whether a color attacks a square;
  `isInCheck(color)` uses it to check the king.
- `isPseudoLegalMove()` and `isLegalMove()` check moves. The latter also
  rejects moves that leave the moving side's king in check.
- `generateLegalMoves()` returns every legal move in the current position.
- `perft(depth)` counts move sequences to a given depth for move-generation
  checking; `perftDivide(depth)` prints the count for each first move.
- `loadFEN(fen)` loads a position from a FEN string.

## `src/board.cpp`

This file implements the board storage and position updates: square lookup,
piece placement/removal, starting-position setup, board printing, attack and
check detection, and making/undoing moves. Keeping these operations here gives
move generation and search one consistent way to read and change a position.

Its file-local helpers are `bit(square)` (make a bit mask for a square),
`pieceIndex(piece)` (convert a piece to its bitboard index), `colorOf(piece)`
(find its color), and `pieceCharacter(piece)` (choose the character used by
`print()`). These helpers are private to this source file.

The move-generation and perft functions are implemented in `src/movegen.cpp`,
and `loadFEN()` is implemented in `src/fen.cpp`; they are declared in
`board.h` because they operate on `Board`.

## Square numbering

Squares use indices `0` through `63`, starting at `a1` and moving across each
rank: `a1 = 0`, `b1 = 1`, ..., `h1 = 7`, `a2 = 8`, ..., `h8 = 63`.
