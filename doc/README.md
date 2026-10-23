# Cerebrum code notes

Short explanations of the engine files and their functions. Start with
`board.md` for the position data, then use the page for the file you are
working on.

- [How the engine works from startup to best move](engine-flow.md)

- [Board data and operations](board.md)
- [Move generation](movegen.md)
- [FEN positions](fen.md)
- [Evaluation](eval.md)
- [Search](search.md)
- [Transposition table and position hashes](tt.md)
- [UCI interface](uci.md)
- [Console game and program entry](main.md)
- [Tests](tests.md)

The declarations are in `include/`; most implementations are in the matching
`src/` files. `board.md` covers `board.h` and `board.cpp`.
