#pragma once 
#include "board.h"
#include <cstdint>
#include <vector>

struct SearchResult{
    Move bestMove;
    int score=0;
    std::uint64_t nodes=0;
    std::uint64_t transpositionHits=0;
    std::uint64_t repetitionDraws=0;
    bool hasMove=false;
    int completedDepth=0;
};
SearchResult findBestMove(
    Board& board,
    int depth,
    int timeLimitMs = 1000,
    const std::vector<std::uint64_t>& gameHistory = {});
bool isRepeatedPosition(std::uint64_t key,
                        const std::vector<std::uint64_t>& history);
