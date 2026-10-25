import re
import math
from glob import glob

# Parse all PGN results
results = []
for pgn in glob("/private/tmp/rating_test/*.pgn"):
    with open(pgn, "r") as f:
        content = f.read()
    games = content.split('[Event ')
    for g in games:
        if not g.strip(): continue
        white_m = re.search(r'\[White "([^"]+)"\]', g)
        black_m = re.search(r'\[Black "([^"]+)"\]', g)
        res_m = re.search(r'\[Result "([^"]+)"\]', g)
        if white_m and black_m and res_m:
            w = white_m.group(1)
            b = black_m.group(1)
            r = res_m.group(1)
            if r in ["1-0", "0-1", "1/2-1/2"]:
                results.append((w, b, r))

print(f"Total valid games parsed: {len(results)}")

# Known Elo of opponents
known_elo = {
    "Stockfish1320": 1320,
    "Stockfish1350": 1350,
    "Stockfish1425": 1425,
    "Stockfish1500": 1500,
    "Stockfish1575": 1575,
    "Stockfish1650": 1650,
}

# Collect Cerebrum encounters
cerebrum_games = []
for w, b, r in results:
    if w == "Cerebrum" and b in known_elo:
        score = 1.0 if r == "1-0" else (0.5 if r == "1/2-1/2" else 0.0)
        cerebrum_games.append((known_elo[b], score))
    elif b == "Cerebrum" and w in known_elo:
        score = 1.0 if r == "0-1" else (0.5 if r == "1/2-1/2" else 0.0)
        cerebrum_games.append((known_elo[w], score))

print(f"Total Cerebrum games against calibrated Stockfish: {len(cerebrum_games)}")

# Compute Bradley-Terry MLE for Cerebrum rating R
# Expected score against opponent with rating E: P = 1 / (1 + 10^((E - R)/400))
def total_expected(R):
    return sum(1.0 / (1.0 + 10.0 ** ((E - R) / 400.0)) for E, _ in cerebrum_games)

actual_score = sum(s for _, s in cerebrum_games)
print(f"Actual score: {actual_score} / {len(cerebrum_games)} ({actual_score/len(cerebrum_games)*100:.1f}%)")

# Binary search for R where total_expected(R) == actual_score
low, high = 500.0, 2500.0
for _ in range(100):
    mid = (low + high) / 2.0
    if total_expected(mid) < actual_score:
        low = mid
    else:
        high = mid

est_elo = mid

# Compute Fisher information to get standard error
# dP/dR = P * (1 - P) * (ln 10 / 400)
# I(R) = sum (dP/dR)^2 / (P*(1-P)) = sum P*(1-P) * (ln 10 / 400)^2
scale = math.log(10) / 400.0
info = sum((1.0 / (1.0 + 10.0 ** ((E - est_elo) / 400.0))) * (1.0 - (1.0 / (1.0 + 10.0 ** ((E - est_elo) / 400.0)))) * (scale ** 2) for E, _ in cerebrum_games)
std_err = 1.0 / math.sqrt(info) if info > 0 else 0

print(f"Estimated Cerebrum Elo: {est_elo:.1f} ± {1.96 * std_err:.1f} (95% CI)")

# Breakdown by opponent
by_opp = {}
for w, b, r in results:
    if w == "Cerebrum" and b in known_elo:
        by_opp.setdefault(b, [0, 0, 0])
        if r == "1-0": by_opp[b][0] += 1
        elif r == "1/2-1/2": by_opp[b][1] += 1
        else: by_opp[b][2] += 1
    elif b == "Cerebrum" and w in known_elo:
        by_opp.setdefault(w, [0, 0, 0])
        if r == "0-1": by_opp[w][0] += 1
        elif r == "1/2-1/2": by_opp[w][1] += 1
        else: by_opp[w][2] += 1

print("\nBreakdown by opponent:")
for opp, (w, d, l) in sorted(by_opp.items()):
    total = w + d + l
    pct = (w + 0.5*d)/total * 100
    print(f"  vs {opp} (Elo {known_elo[opp]}): {w}W - {d}D - {l}L ({pct:.1f}%)")
