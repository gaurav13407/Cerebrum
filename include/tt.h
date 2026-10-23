#pragma once
#include "board.h"
#include <cstddef>
#include <cstdint>
#include <vector>

enum class TTBound : std::uint8_t {
  Exact,
  LowerBound,
  UpperBound

};

struct TTEntry {
  std::uint64_t key = 0;
  int depth = -1;
  int score = 0;
  TTBound bound = TTBound::Exact;
  Move bestMove{};
  bool valid = false;
};

std::uint64_t zobristHash(const Board &board);
std::uint64_t transpositionHash(const Board& board);
int scoreToTable(int score, int ply);
int scoreFromTable(int score, int ply);

class TranspositionTable {
public:
  explicit TranspositionTable(std::size_t capacity = 1u << 20);
  void clear();
  bool probe(std::uint64_t key, TTEntry &entry) const;
  void store(std::uint64_t key, const TTEntry &entry);

private:
  std::vector<TTEntry> entries_;
};
