# Tests: `tests/test_board.cpp`

The test program's `main()` runs assertions covering board updates and legal
move generation, special moves, FEN loading, make/unmake restoration,
evaluation, search, and transposition-table behavior. It also runs known perft
positions to compare generated move counts with expected counts.

If every assertion succeeds, the program prints the test section results and
exits successfully. If an assertion fails, it stops at the failing check. Build
and run the suite with `make test` from the project folder.
