#pragma once

#include <deque>
#include <functional>  // For std::hash
#include <initializer_list>
#include <optional>  // For std::nullopt
#include <unordered_set>
#include <utility>  // For std::swap

#if __cplusplus >= 201703L
#define NOEXCEPT_CXX17 noexcept
#else
#define NOEXCEPT_CXX17
#endif

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
class deque_of_unique {
 public:
  // *Member types
  using value_type = T;
  using key_type = T;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using const_reference = const value_type&;
  using deque_type = std::deque<T>;
  using unordered_set_type = std::unordered_set<T, Hash, KeyEqual>;
  using size_type = typename deque_type::size_type;
  using const_iterator = typename deque_type::const_iterator;
  using iterator = const_iterator;
  using reverse_iterator = typename deque_type::reverse_iterator;
  using const_reverse_iterator = typename deque_type::const_reverse_iterator;

  // Member functions
  // Constructor
  deque_of_unique() = default;

  template <class input_it>
  deque_of_unique(input_it first, input_it last) {
    _push_back(first, last);
  }

  deque_of_unique(const std::initializer_list<T>& init)
      : deque_of_unique(init.begin(), init.end()) {}

  deque_of_unique(const deque_of_unique& other) { _push_back(other); }

  deque_of_unique(deque_of_unique&& other) {
    std::swap(deque_, other.deque_);
    std::swap(set_, other.set_);
  }

  deque_of_unique& operator=(const deque_of_unique& other) = default;
  deque_of_unique& operator=(deque_of_unique&& other) NOEXCEPT_CXX17 = default;
  deque_of_unique& operator=(std::initializer_list<T> ilist) {
    deque_of_unique temp(ilist);
    std::swap(deque_, temp.deque_);
    std::swap(set_, temp.set_);
    return *this;
  }

  template <class input_it>
  void assign(input_it first, input_it last) {
    clear();
    _push_back(first, last);
  }

  void assign(std::initializer_list<T> ilist) {
    clear();
    _push_back(ilist.begin(), ilist.end());
  }

  // Element access
  const_reference at(size_type pos) const { return deque_.at(pos); }
  const_reference front() const { return deque_.front(); }
  const_reference operator[](size_type pos) const { return deque_[pos]; }
  const_reference back() const { return deque_.back(); }

  // Iterators
  const_iterator cbegin() const noexcept { return deque_.cbegin(); }
  const_iterator cend() const noexcept { return deque_.cend(); }

  iterator begin() const noexcept { return deque_.cbegin(); }
  iterator end() const noexcept { return deque_.cend(); }

  const_reverse_iterator crbegin() const noexcept { return deque_.crbegin(); }
  const_reverse_iterator crend() const noexcept { return deque_.crend(); }

  // Modifiers
  void clear() noexcept {
    deque_.clear();
    set_.clear();
  }

  // Precondition: pos must be a valid and dereferenceable iterator of this
  // container (i.e. pos != cend()). Violating this is undefined behaviour,
  // matching the contract of std::deque::erase.
  const_iterator erase(const_iterator pos) {
    set_.erase(*pos);
    return deque_.erase(pos);
  }

  const_iterator erase(const_iterator first, const_iterator last) {
    if (first == last) {
      return last;
    }

    for (auto it = first; it != last; ++it) {
      set_.erase(*it);
    }

    return deque_.erase(first, last);
  }

  std::pair<const_iterator, bool> insert(const_iterator pos, const T& value) {
    if (set_.insert(value).second) {
      return std::make_pair(deque_.insert(pos, value), true);
    }
    return std::make_pair(pos, false);
  }

  std::pair<const_iterator, bool> insert(const_iterator pos, T&& value) {
    if (set_.insert(value).second) {
      return std::make_pair(deque_.insert(pos, std::move(value)), true);
    }
    return std::make_pair(pos, false);
  }

  template <class input_it>
  const_iterator insert(const_iterator pos, input_it first, input_it last) {
    auto pos_index = pos - deque_.cbegin();
    auto first_inserted_index = pos_index;
    auto temp_pos = deque_.begin() + (pos - deque_.cbegin());
    auto any_inserted = false;

    for (auto it = first; it != last; ++it) {
      if (set_.insert(*it).second) {
        temp_pos = deque_.insert(temp_pos, *it);
        if (!any_inserted) {
          first_inserted_index = temp_pos - deque_.cbegin();
          any_inserted = true;
        }
        ++temp_pos;
      }
    }
    return any_inserted ? first_inserted_index + deque_.cbegin()
                        : pos_index + deque_.cbegin();
  }

  const_iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  template <class... Args>
  std::pair<const_iterator, bool> emplace(const_iterator pos, Args&&... args) {
    if (set_.emplace(args...).second) {
      return std::make_pair(deque_.emplace(pos, std::forward<Args>(args)...),
                            true);
    }
    return std::make_pair(pos, false);
  }

#if __cplusplus < 201703L
  template <class... Args>
  void emplace_front(Args&&... args) {
    if (set_.emplace(args...).second) {
      deque_.emplace_front(std::forward<Args>(args)...);
    }
  }
#else
  template <class... Args>
  std::optional<std::reference_wrapper<T>> emplace_front(Args&&... args) {
    if (set_.emplace(args...).second) {
      return deque_.emplace_front(std::forward<Args>(args)...);
    }
    return std::nullopt;
  }
#endif

#if __cplusplus < 201703L
  template <class... Args>
  void emplace_back(Args&&... args) {
    if (set_.emplace(args...).second) {
      deque_.emplace_back(std::forward<Args>(args)...);
    }
  }
#else
  template <class... Args>
  std::optional<std::reference_wrapper<T>> emplace_back(Args&&... args) {
    if (set_.emplace(args...).second) {
      return deque_.emplace_back(std::forward<Args>(args)...);
    }
    return std::nullopt;
  }
#endif

  void pop_front() {
    if (!deque_.empty()) {
      const auto& f = deque_.front();
      set_.erase(f);
      deque_.pop_front();
    }
  }

  void pop_back() {
    if (!deque_.empty()) {
      const auto& f = deque_.back();
      set_.erase(f);
      deque_.pop_back();
    }
  }

  bool push_front(const T& value) {
    if (set_.insert(value).second) {
      deque_.push_front(value);
      return true;
    }
    return false;
  }

  bool push_front(T&& value) {
    if (set_.count(value) > 0) {
      return false;
    }
    set_.insert(value);
    deque_.push_front(std::move(value));
    return true;
  }

  bool push_back(const T& value) {
    if (set_.insert(value).second) {
      deque_.push_back(value);
      return true;
    }
    return false;
  }

  bool push_back(T&& value) {
    if (set_.count(value) > 0) {
      return false;
    }
    set_.insert(value);
    deque_.push_back(std::move(value));
    return true;
  }

#if __cplusplus >= 202302L
  template <std::ranges::input_range R>
  void prepend_range(R&& rng) {
    // Collect elements not already in the container, deduplicating within the
    // range while preserving first-occurrence order, then prepend in reverse.
    std::unordered_set<std::ranges::range_value_t<R>, Hash, KeyEqual> pending;
    std::deque<std::ranges::range_value_t<R>> tmp;
    for (auto&& v : std::forward<R>(rng)) {
      if (set_.count(v) == 0 && pending.insert(v).second) {
        tmp.push_back(v);
      }
    }
    for (auto it = tmp.rbegin(); it != tmp.rend(); ++it)
      push_front(std::move(*it));
  }

  template <std::ranges::input_range R>
  void append_range(R&& rng) {
    for (auto&& v : std::forward<R>(rng))
      push_back(std::forward<decltype(v)>(v));
  }
#endif

 private:
  template <class input_it>
  void _push_back(input_it first, input_it last) {
    while (first != last) {
      push_back(*first++);
    }
  }

  bool _push_back(const deque_of_unique<T, Hash>& other) {
    return _push_back(other.deque_);
  }

  bool _push_back(const std::deque<T>& other) {
    bool any_added = false;
    for (const auto& entry : other) {
      auto added = push_back(entry);
      any_added = any_added || added;
    }
    return any_added;
  }

 public:
  void swap(deque_of_unique& other) NOEXCEPT_CXX17 {
    deque_.swap(other.deque_);
    set_.swap(other.set_);
  }

  // Capacity
  bool empty() const noexcept { return deque_.empty(); }

  size_type size() const noexcept { return deque_.size(); }

// Look up
#if __cplusplus < 202002L
  const_iterator find(const T& x) const {
    if (set_.count(x) == 0) {
      return cend();
    }
    auto it = cbegin();
    while (it != cend()) {
      if (*it == x) {
        return it;
      }
      it++;
    }
    return cend();
  }
#else
  const_iterator find(const T& x) const {
    if (set_.count(x) == 0) {
      return cend();
    }
    auto it = cbegin();
    while (it != cend()) {
      if (*it == x) {
        return it;
      }
      it++;
    }
    return cend();
  }

  template <class K>
    requires requires {
      typename Hash::is_transparent;
      typename KeyEqual::is_transparent;
    }
  const_iterator find(const K& x) const {
    if (set_.count(x) == 0) {
      return cend();
    }
    KeyEqual eq{};
    auto it = cbegin();
    while (it != cend()) {
      if (eq(*it, x)) {
        return it;
      }
      it++;
    }
    return cend();
  }
#endif

#if __cplusplus >= 202002L
  bool contains(const key_type& key) const { return set_.contains(key); }

  template <class K>
    requires requires {
      typename Hash::is_transparent;
      typename KeyEqual::is_transparent;
    }
  bool contains(const K& x) const {
    return set_.contains(x);
  }
#endif

  std::pair<const_iterator, const_iterator> equal_range(
      const key_type& key) const {
    auto it = find(key);
    if (it == cend()) return {cend(), cend()};
    return {it, it + 1};
  }

#if __cplusplus >= 202002L
  template <class K>
    requires requires {
      typename Hash::is_transparent;
      typename KeyEqual::is_transparent;
    }
  std::pair<const_iterator, const_iterator> equal_range(const K& x) const {
    auto it = find(x);
    if (it == cend()) return {cend(), cend()};
    return {it, it + 1};
  }
#endif

  // Destructor
  ~deque_of_unique() = default;

  // Get member variables
  const deque_type& deque() const { return deque_; }
  const unordered_set_type& set() const { return set_; }

 private:
  deque_type deque_;
  unordered_set_type set_;
};  // class deque_of_unique

// Non-member function
#if __cplusplus >= 202002L && __cplusplus < 202600L
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class U>
typename deque_of_unique<T, Hash, KeyEqual>::size_type erase(
    deque_of_unique<T, Hash, KeyEqual>& c, const U& value) {
  auto it = c.find(value);
  if (it != c.cend()) {
    c.erase(it);
    return 1;
  }
  return 0;
}
#else
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class U = T>
typename deque_of_unique<T, Hash, KeyEqual>::size_type erase(
    deque_of_unique<T, Hash, KeyEqual>& c, const U& value) {
  auto it = c.find(value);
  if (it != c.cend()) {
    c.erase(it);
    return 1;
  }
  return 0;
}
#endif

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class Pred>
typename deque_of_unique<T, Hash, KeyEqual>::size_type erase_if(
    deque_of_unique<T, Hash, KeyEqual>& c, Pred pred) {
  auto it = c.cbegin();
  typename deque_of_unique<T, Hash, KeyEqual>::size_type r = 0;
  while (it != c.cend()) {
    if (pred(*it)) {
      it = c.erase(it);
      ++r;
    } else {
      ++it;
    }
  }
  return r;
}

// Operators
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator==(const deque_of_unique<T, Hash, KeyEqual>& lhs,
                const deque_of_unique<T, Hash, KeyEqual>& rhs) {
  return (lhs.deque() == rhs.deque());
}

#if __cplusplus < 202002L
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator!=(const deque_of_unique<T, Hash, KeyEqual>& lhs,
                const deque_of_unique<T, Hash, KeyEqual>& rhs) {
  return (lhs.deque() != rhs.deque());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator<(const deque_of_unique<T, Hash, KeyEqual>& lhs,
               const deque_of_unique<T, Hash, KeyEqual>& rhs) {
  return (lhs.deque() < rhs.deque());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator<=(const deque_of_unique<T, Hash, KeyEqual>& lhs,
                const deque_of_unique<T, Hash, KeyEqual>& rhs) {
  return (lhs.deque() <= rhs.deque());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator>(const deque_of_unique<T, Hash, KeyEqual>& lhs,
               const deque_of_unique<T, Hash, KeyEqual>& rhs) {
  return (lhs.deque() > rhs.deque());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator>=(const deque_of_unique<T, Hash, KeyEqual>& lhs,
                const deque_of_unique<T, Hash, KeyEqual>& rhs) {
  return (lhs.deque() >= rhs.deque());
}
#else
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
auto operator<=>(const deque_of_unique<T, Hash, KeyEqual>& lhs,
                 const deque_of_unique<T, Hash, KeyEqual>& rhs) {
  return (lhs.deque() <=> rhs.deque());
}
#endif
};  // namespace containerofunique
