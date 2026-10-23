# UCI protocol: `uci.h` and `uci.cpp`

UCI (Universal Chess Interface) lets chess GUIs communicate with Cerebrum using
text commands. `runUci()` reads commands from standard input and writes
responses to standard output. The GUI sends positions and search limits; the
engine reports its search information and `bestmove`.

## Public entry point

- `runUci()` initializes the starting board and position history, then handles
  `uci`, `isready`, `ucinewgame`, `position`, `go`, and `quit` commands. It
  returns when asked to quit or when input closes.

## File-local helpers

- `promotionPiece(symbol, sideToMove)` converts UCI's `q`, `r`, `b`, or `n`
  suffix into the correct colored promotion piece.
- `parseUciMove(text, board, parsed)` converts coordinate notation such as
  `e2e4` or `e7e8q` into a move, accepting it only if it is legal on the board.
- `moveText(move)` converts an engine move back to UCI coordinate notation.
- `setPosition(tokens, board, history)` reads a `position startpos` or
  `position fen ...` command, applies any following moves, and updates the
  board and its position-hash history. Invalid input leaves the old state in
  place.
- `parsePositive(value, fallback)` parses a positive integer or returns the
  supplied fallback.
- `searchAndReport(board, history, tokens)` reads the `go` limits, calls
  `findBestMove()`, and prints an `info` line plus the selected `bestmove`.

The current search runs synchronously. `stop` is recognized, but cannot
interrupt a search while it is running; search ends at its time limit and
reports its move.
