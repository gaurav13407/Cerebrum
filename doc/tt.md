# Position hashes and transposition table: `tt.h` and `tt.cpp`

Search can reach the same position by different move orders. A position hash
lets it recognize the position and reuse saved search information.

## Hash and score functions

- `zobristHash(board)` combines deterministic random keys for each piece and
  square, side to move, castling rights, and en passant file. It identifies a
  chess position for repetition checks.
- `transpositionHash(board)` adds the halfmove clock to the position hash, so
  transposition-table entries distinguish positions with different progress
  toward the fifty-move draw.
- `scoreToTable(score, ply)` adjusts mate scores before saving them, so mate
  distance remains correct when the same entry is reused at another search
  depth in the tree.
- `scoreFromTable(score, ply)` reverses that adjustment when a score is read.

## Table types and methods

- `TTBound` labels an entry as an exact score, a lower bound, or an upper bound.
  Search uses these bounds with alpha-beta pruning.
- `TTEntry` stores the position key, searched depth, score, bound, best move,
  and whether the slot contains valid data.
- `TranspositionTable(capacity)` creates a fixed-size table. A zero capacity is
  changed to one slot.
- `clear()` resets every slot.
- `probe(key, entry)` looks in the slot selected by the key and copies the entry
  out only if it is valid and its full key matches.
- `store(key, entry)` saves an entry in that slot, replacing an empty slot, the
  same key, or an entry searched to no greater depth.

## File-local helpers

- `splitMix64(state)` produces the deterministic pseudo-random numbers used for
  Zobrist keys.
- `getZobristKeys()` creates the key arrays once and returns the same set for
  every hash calculation.
