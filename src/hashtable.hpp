#ifndef HASHTABLE_HPP_INCLUDED
#define HASHTABLE_HPP_INCLUDED

#include "assert.hpp"
#include "move.hpp"

enum FLAGS
{
    EXACT = 0,
    LOWERBOUND,
    UPPERBOUND
};

struct Entry {
   public:
    Entry() : key_(0ULL), move_{}, depth_(0), eval_(0), flag_(FLAGS::EXACT) {
    }

    Entry(const uint64_t key,
          const Move move,
          const int depth,
          const int eval,
          const int flag)
        : key_(key), move_(move), depth_(depth), eval_(eval), flag_(flag) {
        UCI_ASSERT(depth_ >= 0);
        UCI_ASSERT(flag_ == FLAGS::EXACT || flag_ == FLAGS::LOWERBOUND ||
               flag_ == FLAGS::UPPERBOUND);
        UCI_ASSERT(-INF - MAX_DEPTH <= eval);
        UCI_ASSERT(eval <= INF + MAX_DEPTH);
    }

    uint64_t key() const {
        return key_;
    }
    Move move() const {
        return move_;
    }
    int depth() const {
        return depth_;
    }
    int eval() const {
        return eval_;
    }
    uint8_t flag() const {
        return flag_;
    }

    bool operator==(const Entry &rhs) const {
        return key_ == rhs.key_ && move_ == rhs.move_ && depth_ == rhs.depth_ &&
               eval_ == rhs.eval_ && flag_ == rhs.flag_;
    }

   private:
    uint64_t key_;
    Move move_;
    int depth_;
    int eval_;
    int flag_;
};

class Hashtable {
   public:
    Hashtable() : num_entries(0), entries(nullptr) {
    }

    explicit Hashtable(const int mb) : num_entries(0), entries(nullptr) {
        create(mb);
    }

    void create(const int mb) {
        num_entries = mb * (1024 * 1024) / sizeof(Entry);
        entries = new Entry[num_entries]();
        UCI_ASSERT(entries);
    }

    ~Hashtable() {
        if (entries) {
            delete entries;
        }
    }

    void clear() {
        for (int n = 0; n < num_entries; ++n) {
            entries[n] = Entry(0ULL, NO_MOVE, 0, 0, 0);
        }
    }

    int index(const uint64_t key) const {
        UCI_ASSERT(num_entries > 0);
        return key % num_entries;
    }

    Entry probe(const uint64_t key) const {
        int idx = index(key);
        return entries[idx];
    }

    void add(const uint64_t key,
             const int depth,
             const int eval,
             const Move move,
             const int flag) {
        int idx = index(key);
        entries[idx] = Entry(key, move, depth, eval, flag);
    }

   private:
    int num_entries;
    Entry *entries;
};

int eval_to_tt(int eval, int ply);
int eval_from_tt(int eval, int ply);

#endif
