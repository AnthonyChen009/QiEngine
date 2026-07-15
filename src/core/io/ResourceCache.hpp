#pragma once

#include <unordered_map>
#include <memory>
#include <optional>

template<typename Key, typename Resource>
class ResourceCache {
public:
    ResourceCache() = default;

    std::shared_ptr<Resource> get(const Key& key) const {
        auto it = m_cache.find(key);
        if (it == m_cache.end()) {
            return nullptr;
        }
        return it->second;
    }

    bool has(const Key& key) const {
        return m_cache.find(key) != m_cache.end();
    }


    void insert(const Key& key, std::shared_ptr<Resource> resource) {
        m_cache[key] = std::move(resource);
    }

    bool remove(const Key& key) {
        return m_cache.erase(key) > 0;
    }

    void clear() {
        m_cache.clear();
    }

    size_t size() const {
        return m_cache.size();
    }

    bool empty() const {
        return m_cache.empty();
    }

    void removeUnused() {
        for (auto it = m_cache.begin(); it != m_cache.end(); ) {
            if (it->second.use_count() == 1) {
                it = m_cache.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    std::unordered_map<Key, std::shared_ptr<Resource>> m_cache;
};
