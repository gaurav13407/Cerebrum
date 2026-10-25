# ♟️ Cerebrum — Benchmark Rating Report

**Date Tested:** October 25, 2026  
**Test Framework:** `cutechess-cli 1.5.1`  
**Reference Engine:** Stockfish 19 (Calibrated via `UCI_LimitStrength` & `UCI_Elo`)  
**Time Control:** `4s + 0.1s` increment per move  
**Total Games Played:** 30 games across 6 Elo tiers  

---

## 🏆 Rating Summary

| Metric | Value |
| :--- | :--- |
| **Estimated Elo (FIDE / CCRL equivalent)** | **~1200 – 1250 Elo** |
| **Bradley-Terry Maximum Likelihood Estimate (MLE)** | **1189.4 ± 143.5 Elo** |
| **95% Confidence Interval** | **1046 – 1333 Elo** |
| **Performance vs Calibrated Stockfish 1320** | **40.0% (8 Wins, 12 Losses)** → **~1250 Elo** |
| **Crash / Illegal Move Rate** | **0.0%** (Clean 30/30 games) |

---

## 📊 Match Results Breakdown

All matches used opening suites to eliminate color and opening bias (each pair was played once as White and once as Black).

| Opponent | Reference Elo | Cerebrum Score | Win Rate | Performance Elo |
| :--- | :---: | :---: | :---: | :---: |
| **Stockfish 1320** | 1320 | **8W – 0D – 12L** | **40.0%** | **~1250 Elo** |
| **Stockfish 1350** | 1350 | **0W – 0D – 2L** | 0.0% | < 1350 Elo |
| **Stockfish 1425** | 1425 | **0W – 0D – 2L** | 0.0% | < 1400 Elo |
| **Stockfish 1500** | 1500 | **0W – 0D – 2L** | 0.0% | < 1400 Elo |
| **Stockfish 1575** | 1575 | **0W – 0D – 2L** | 0.0% | < 1400 Elo |
| **Stockfish 1650** | 1650 | **0W – 0D – 2L** | 0.0% | < 1400 Elo |
| **Total** | — | **8W – 0D – 22L** | **26.7%** | **~1190 Elo** |

---

## 🔍 Technical Observations

### Strengths
1. **Robust Engine Stability:**  
   The engine played 30 consecutive games without any crashes, timeouts, illegal moves, or protocol desyncs with Cutechess.
2. **Tactical Vision at 1200–1300 Level:**  
   Cerebrum demonstrated strong short-range tactics, delivering direct checkmate against Stockfish 1320 in several games where Stockfish left king-safety weaknesses.
3. **Effective Search Architecture:**  
   Iterative deepening, alpha-beta pruning, quiescence search, and the transposition table function as expected.

### Areas for Improvement (Reaching 1500+ / 1800+ Elo)
1. **Null Move Pruning (NMP):**  
   Skipping a move in non-tactical positions to verify if the position is already overwhelmingly winning. Adds ~100–150 Elo in search depth efficiency.
2. **Late Move Reductions (LMR):**  
   Searching quiet moves late in the move list to lower depth. Adds ~100+ Elo.
3. **King Safety Evaluation:**  
   Stockfish won several games by launching middlegame attacks against open or poorly defended castled positions.
4. **Pawn Structure Evaluation:**  
   Passed pawns and doubled/isolated pawns are currently undervalued in endgame transitions.

---

## 📁 Benchmark Artifacts

The games and analysis script are preserved in the `benchmarks/` directory:
- `benchmarks/sf1320_20games.pgn`: The 20-game head-to-head match against Stockfish 1320.
- `benchmarks/gauntlet.pgn`: The multi-tier gauntlet against Stockfish 1350–1650.
- `benchmarks/openings.pgn`: Diverse opening suite used for the tests.
- `benchmarks/calc_elo.py`: Maximum likelihood rating calculator script.

---

## 🔁 How to Re-Run This Benchmark Locally

To test new engine commits against Stockfish 1320:

```bash
/private/tmp/cutechess/build/cutechess-cli \
  -engine name=Cerebrum cmd=$(pwd)/cerebrum proto=uci \
  -engine name=Stockfish1320 cmd=stockfish proto=uci option.UCI_LimitStrength=true option.UCI_Elo=1320 \
  -each tc=4+0.1 \
  -games 20 \
  -repeat \
  -openings file=benchmarks/openings.pgn format=pgn order=random \
  -draw movenumber=40 movecount=6 score=15 \
  -resign movecount=4 score=700 \
  -concurrency 6 \
  -pgnout benchmarks/latest_test.pgn
```
