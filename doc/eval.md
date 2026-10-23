# Evaluation: `eval.h` and `eval.cpp`

Evaluation gives a number to a position so search can compare possible moves.
The current evaluator only counts material; it does not yet score activity,
king safety, pawn structure, or positional features.

- `evaluateMaterial(board)` counts each side's pieces using the values Pawn
  100, Knight 320, Bishop 330, Rook 500, Queen 900, King 0. It returns the
  difference from the side-to-move's point of view: positive is good for the
  player whose turn it is; negative is good for the opponent.

`eval.h` declares the function for search and other files. `eval.cpp` contains
its implementation.
