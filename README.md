# ♟️ Cerebrum Chess Engine

A chess engine written in C++ from scratch. It uses bitboards, legal move
generation, iterative-deepening alpha-beta search, quiescence search, and a
transposition table.

---

## Features

- **Bitboard representation** — 64-bit integers for fast move generation
- **Legal move generation** — all piece types including castling, en passant, promotions
- **Alpha-Beta search** with iterative deepening
- **Transposition table** using Zobrist hashing
- **Move ordering** — search move ordering heuristics
- **Quiescence search** — avoids horizon effect on captures
- **Evaluation** — current basic material and positional evaluation
- **UCI protocol** — supports UCI GUIs such as Arena and Cute Chess

---

## Build

```bash
git clone https://github.com/yourusername/cerebrum
cd cerebrum
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Usage

Build, then select the `cerebrum` executable in a UCI-compatible GUI (such as
Arena or Cute Chess). The engine speaks UCI by default.

```bash
./cerebrum
```

For a simple terminal game against the engine, run:

```bash
./cerebrum --console
```

---

## Architecture
src/
├── board.h / board.cpp       # Board state, Zobrist hashing
├── movegen.h / movegen.cpp   # Legal move generation (bitboards)
├── search.h / search.cpp     # Alpha-beta, iterative deepening
├── eval.h / eval.cpp         # Position evaluation
├── tt.h / tt.cpp             # Transposition table
├── uci.h / uci.cpp           # UCI protocol handler
└── main.cpp                  # Entry point

---

## Roadmap

- [x] Board representation (bitboards)
- [x] Move generation
- [x] Alpha-beta search
- [x] Transposition table
- [x] Basic UCI protocol
- [ ] NNUE evaluation
- [ ] Endgame tablebases (Syzygy)
- [ ] Lichess bot deployment

---

## Resources

- [Chess Programming Wiki](https://www.chessprogramming.org)
- [Stockfish source](https://github.com/official-stockfish/Stockfish)
- [Sebastian Lague's Chess series](https://www.youtube.com/c/SebastianLague)

---

## License

MIT
# Cerebrum
