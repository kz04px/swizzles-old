#ifndef CACHE_LRU_HPP
#define CACHE_LRU_HPP

#include <cassert>
#include <list>
#include <unordered_map>

namespace cache {

template <typename Key, typename Value>
class lru {
   public:
    typedef typename std::pair<Key, Value> key_value_pair;
    typedef typename std::list<key_value_pair>::iterator list_iterator;

    lru(const unsigned int n) : max_entries_(n) {
    }

    size_t size() const {
        return entries_list_.size();
    }

    size_t max_size() const {
        return max_entries_;
    }

    void add(const Key &k, const Value &v) {
        // Remove the entry if it already exists
        auto it = entries_map_.find(k);
        if (it != entries_map_.end()) {
            entries_list_.erase(it->second);
            entries_map_.erase(it);
        }
        // Remove the last entry if we're already maxed out
        else if (size() >= max_entries_) {
            const key_value_pair pair = entries_list_.back();

            assert(entries_map_.find(pair.first) != entries_map_.end());

            entries_list_.pop_back();
            entries_map_.erase(pair.first);
        }

        assert(!has(k));

        // Add entry
        entries_list_.push_front(key_value_pair(k, v));
        entries_map_.insert({k, entries_list_.begin()});

        assert(has(k));
    }

    std::optional<Value *> get(const Key &k) {
        auto it = entries_map_.find(k);

        // Not found
        if (it == entries_map_.end()) {
            return std::nullopt;
        }

        entries_list_.splice(entries_list_.begin(), entries_list_, it->second);

        return &it->second->second;
    }

    bool has(const Key &k) const {
        return entries_map_.find(k) != entries_map_.end();
    }

   private:
    const size_t max_entries_;
    std::list<key_value_pair> entries_list_;
    std::unordered_map<Key, list_iterator> entries_map_;
};

}  // namespace cache

#endif
