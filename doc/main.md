# Program entry and console play: `main.cpp`

Without `--console`, `main()` starts the UCI loop for a chess GUI. With
`--console`, it starts a simple human-versus-engine terminal game.

## File-local helpers

- `promotionPiece(symbol, sideToMove)` converts a promotion letter into the
  correct colored piece.
- `parseMove(input, legalMoves, sideToMove, selectedMove)` converts coordinate
  input such as `e2e4` into a move, and accepts it only if it appears in the
  current legal-move list.
- `moveText(move)` prints an engine move in coordinate form, including a
  promotion suffix when needed.

## Entry point

- `main(argc, argv)` selects UCI or console mode. Console mode sets the initial
  position, records each position hash, asks the human for White's moves, asks
  search for Black's moves, and stops on checkmate, stalemate, supported draw,
  or `quit`.

The console search currently requests depth 6 with a 1000 ms time limit per
move. These are constants near the start of `main()`.
