#include <unordered_map>
#include <list>

namespace cppomm {
namespace cache {

// 淘汰时回调
template <class K, class V>
using OnEvict = void (*)(const K& key, const V& value);

// Key-Value对
template <class K, class V>
using Entry = std::pair<K, V>;

// 最近最少使用缓存
// 优点：稳定淘汰
// 缺点：需要链表和大量的链表操作
template <class K, class V>
class LRUCache {
public:
    LRUCache(size_t capacity) : LRUCache(capacity, nullptr) {}
    LRUCache(size_t capacity, OnEvict<K, V> on_evict) : capacity_(capacity), on_evict_(on_evict) {
      // 容量得大于0才有意义
      assert(capacity > 0);
    }
    
    // 添加或更新元素
    void Put(const K& key, const V& value) {
        // 如果 key 已经存在，直接把它移到最前面，然后设置新值
        if (auto it = entries_.find(key); it != entries_.end()) {
            evict_list_.splice(evict_list_.begin(), evict_list_, it->second);
            it->second->second = value;
            return;
        }

        // 如果已经到达最大尺寸，先剔除一个元素
        if (Full()) {
            Evict();
        }

        // 添加元素
        evict_list_.emplace_front(Entry{key, value});
        entries_[key] = evict_list_.begin();
    }

    // 获取元素
    std::optional<V> Get(const K& key) {
        // 如果存在移动到头部，然后返回
        if (auto it = entries_.find(key); it != entries_.end()) {
            evict_list_.splice(evict_list_.begin(), evict_list_, it->second);
            return it->second->second;
        }
        // 不存在返回空
        return std::nullopt;
    } 

    // 清空缓存
    // need_on_evict 是否需要触发回调
    void Clear(bool need_on_evict) {
        // 触发回调
        if (need_on_evict && on_evict_) {
            for (auto it = evict_list_.rbegin(); it != evict_list_.rend(); ++it) {
                on_evict_(it->first, it->second);
            }
        }
        // 清空
        entries_.clear();
        evict_list_.clear();
    }

    // 是否包含元素
    bool Contains(const K& key) { return entries_.find(key) != entries_.end(); }

    // 当前元素个数
    size_t Size() const { return entries_.size(); }

    // 容量
    size_t Capacity() const { return capacity_; }

    // 是否满了
    bool Full() const { return Size() == Capacity(); }

private: 
    // 淘汰元素
    // 没有检查容量，请保证有元素可以淘汰
    void Evict() {
        // 回调
        if (on_evict_) {
          on_evict_(evict_list_.back().first, evict_list_.back().second);
        }

        // 淘汰
        entries_.erase(evict_list_.back().first);
        evict_list_.pop_back();
    }

    using Iterator = std::list<Entry<K, V>>::iterator;
    std::unordered_map<K, Iterator> entries_;
    // 淘汰列表
    std::list<Entry<K, V>> evict_list_;
    // 容量
    size_t capacity_;
    OnEvict<K, V> on_evict_;
};
}  // namespace cache
}  // namespace cppomm