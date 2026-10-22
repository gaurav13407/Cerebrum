#pragma once 
#include "board.h"
#include <cstdint>

struct SearchResult{
    Move bestMove;
    int score=0;
    std::uint64_t nodes=0;
    bool hasMove=false;
    int completedDepth=0;
};
SearchResult findBestMove(Board& board, int depth);
