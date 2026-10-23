# Search: `search.h` and `search.cpp`

Search looks ahead through legal moves and chooses the move with the best
score. It uses negamax with alpha-beta pruning, iterative deepening, move
ordering, quiescence search, repetition/draw checks, and the transposition
table. Scores are from the current side-to-move's point of view.

## Public types and functions

- `SearchResult` returns the chosen move and score, visited node count,
  transposition-table hits, repetition draws, whether a move exists, and the
  last fully completed depth.
- `findBestMove(board, depth, timeLimitMs, gameHistory)` searches for a move.
  It progressively searches deeper until reaching `depth` or the time limit,
  and keeps the best move from the last completed depth. `gameHistory` contains
  position hashes from the actual game, used for repetition detection.
- `isRepeatedPosition(key, history)` returns true if the position already
  occurred at least twice in the supplied history (so the current occurrence
  makes three).
- `isInsufficientMaterial(board)` detects the supported automatic draw cases:
  no pawns, rooks, or queens, and only a king, one minor piece, or bishops all
  confined to one square color.

## Search helpers in `search.cpp`

- `SearchControl::shouldStop(nodes)` checks whether the deadline has passed,
  checking the clock every 1024 nodes, and remembers when search should stop.
- `pieceValue(piece)` returns the material value used for ordering captures
  and promotions.
- `isCaptureMove(board, move)` identifies normal and en passant captures for
  quiescence search.
- `moveOrderScore(board, move)` gives captures and promotions high priority so
  alpha-beta is more likely to prune unhelpful branches early.
- `orderMoves(board, moves)` sorts moves by that priority.
- `quiescence(...)` extends leaf search through captures and promotions so
  evaluation is less likely to stop in the middle of an exchange. If in check,
  it searches all legal evasions instead.
- `negamax(...)` searches one node recursively, negates the opponent's score,
  applies alpha-beta cutoffs and draw rules, and probes/stores table entries.
- `findBestMove(...)` initializes the deadline and game history, handles
  terminal/draw positions, and runs the root iterations of iterative deepening.

`MateScore`, `Infinity`, and `SearchControl` are private to the implementation.
`findBestMove()` uses a shared static transposition table between calls.
