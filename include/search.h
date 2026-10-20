#pragma once 
#include "board.h"
#include <cstdint>

struct SearchResult{
    Move bestMove;
    int score=0;
    std::uint64_t nodes=0;
    bool hasMove=false;
};
SearchResult findBestMove(Board& board, int depth);
