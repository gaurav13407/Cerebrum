# FEN loading: `fen.cpp`

FEN is a text format for describing a chess position. `Board::loadFEN(fen)`
reads its six fields: piece placement, side to move, castling rights, en
passant target, halfmove clock, and fullmove number.

The function builds a temporary board while parsing. If a required field or
piece-placement detail is invalid, it returns `false` and leaves the current
board unchanged. On success it replaces the current board and returns `true`.
It uses `putPiece()` so piece and color-occupancy bitboards stay synchronized.

The fullmove number is read as part of the FEN format but is not stored by the
current `Board` structure. The other position fields are stored for move rules,
draw handling, and search.
