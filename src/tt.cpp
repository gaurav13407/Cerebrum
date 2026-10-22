#include "tt.h"

#include <bit>

namespace {
struct ZobristKeys {
    std::uint64_t pieces[12][64]{};
    std::uint64_t sideToMove = 0;
    std::uint64_t castling[16]{};
    std::uint64_t enPassantFile[8]{};
};

std::uint64_t splitMix64(std::uint64_t& state) {
    std::uint64_t value = (state += 0x9e3779b97f4a7c15ULL);
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

const ZobristKeys& getZobristKeys() {
    static const ZobristKeys keys = [] {
        ZobristKeys result;
        std::uint64_t state = 0xC0FFEE123456789ULL;

        for (auto& piece : result.pieces) {
            for (std::uint64_t& square : piece) {
                square = splitMix64(state);
            }
        }
        result.sideToMove = splitMix64(state);

        for (std::uint64_t& right : result.castling) {
            right = splitMix64(state);
        }
        for (std::uint64_t& file : result.enPassantFile) {
            file = splitMix64(state);
        }

        return result;
    }();

    return keys;
}
} // namespace

std::uint64_t zobristHash(const Board& board) {
    const ZobristKeys& keys = getZobristKeys();
    std::uint64_t hash = 0;

    for (int piece = 0; piece < 12; ++piece) {
        std::uint64_t bitboard = board.pieces[piece];

        while (bitboard != 0) {
            const int square = std::countr_zero(bitboard);
            hash ^= keys.pieces[piece][square];
            bitboard &= bitboard - 1;
        }
    }

    if (board.sideToMove == Color::Black) {
        hash ^= keys.sideToMove;
    }

    hash ^= keys.castling[board.castlingRights & 0x0F];

    if (board.enPassantSquare >= 0) {
        hash ^= keys.enPassantFile[board.enPassantSquare % 8];
    }

    return hash;
}

TranspositionTable::TranspositionTable(std::size_t capacity)
    : entries_(capacity == 0 ? 1 : capacity) {}

void TranspositionTable::clear() {
    for (TTEntry& entry : entries_) {
        entry = TTEntry{};
    }
}

bool TranspositionTable::probe(std::uint64_t key, TTEntry& entry) const {
    const TTEntry& stored = entries_[key % entries_.size()];
    if (!stored.valid || stored.key != key) {
        return false;
    }

    entry = stored;
    return true;
}

void TranspositionTable::store(std::uint64_t key, const TTEntry& entry) {
    TTEntry& slot = entries_[key % entries_.size()];

    if (!slot.valid || slot.key == key || entry.depth >= slot.depth) {
        slot = entry;
        slot.key = key;
        slot.valid = true;
    }
}
