#ifndef HASHTABLE_HPP_INCLUDED
#define HASHTABLE_HPP_INCLUDED

#include <cassert>
#include "move.hpp"

enum FLAGS
{
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct Entry {
    uint64_t _key;
    Move _move;
    int _depth;
    int _eval;
    int _flag;

    Entry() {
    }

    Entry(const uint64_t key,
          const Move move,
          const int depth,
          const int eval,
          const int flag)
        : _key(key), _move(move), _depth(depth), _eval(eval), _flag(flag) {
        assert(_depth == depth);
    }

    uint64_t key() {
        return _key;
    }
    Move move() {
        return _move;
    }
    int depth() {
        return _depth;
    }
    int eval() {
        return _eval;
    }
    uint8_t flag() {
        return _flag;
    }

    bool operator==(const Entry &rhs) const {
        return _key == rhs._key && _move == rhs._move && _depth == rhs._depth &&
               _eval == rhs._eval && _flag == rhs._flag;
    }
};

class Hashtable {
   public:
    Hashtable(const int mb) {
        num_entries = mb * (1024 * 1024) / sizeof(Entry);
        entries = new Entry[num_entries];
        assert(entries != NULL);
    }

    ~Hashtable() {
        if (entries) {
            delete entries;
        }
    }

    void clear() {
        for (int n = 0; n < num_entries; ++n) {
            entries[n] = Entry(0ULL, 0, 0, 0, 0);
        }
    }

    int index(const uint64_t key) {
        assert(num_entries != 0);
        return key % num_entries;
    }

    Entry probe(const uint64_t key) {
        int idx = index(key);
        return entries[idx];
    }

    void add(const uint64_t key,
             const int depth,
             const int eval,
             const Move move,
             const int flag) {
        int idx = index(key);
        entries[idx]._key = key;
        entries[idx]._move = move;
        entries[idx]._depth = depth;
        entries[idx]._eval = eval;
        entries[idx]._flag = flag;
        // entries[idx] = Entry(key, move, depth, eval, flag);
    }

   private:
    Entry *entries;
    int num_entries;
};

int eval_to_tt(int eval, int ply);
int eval_from_tt(int eval, int ply);

#endif
