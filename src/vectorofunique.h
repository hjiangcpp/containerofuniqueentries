#pragma once

#include <functional>  // For std::hash
#include <initializer_list>
#include <optional>  // For std::nullopt
#include <unordered_set>
#include <utility>  // For std::swap
#include <vector>

#if __cplusplus >= 201703L
#define NOEXCEPT_CXX17 noexcept
#else
#define NOEXCEPT_CXX17
#endif

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
class vector_of_unique {
 public:
  // *Member types
  using value_type = T;
  using key_type = T;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using const_reference = const value_type &;
  using VectorType = std::vector<T>;
  using UnorderedSetType = std::unordered_set<T, Hash, KeyEqual>;
  using size_type = typename VectorType::size_type;
  using const_iterator = typename VectorType::const_iterator;
  using reverse_iterator = typename VectorType::reverse_iterator;
  using const_reverse_iterator = typename VectorType::const_reverse_iterator;

  // Member functions
  // Constructor
  vector_of_unique() NOEXCEPT_CXX17 = default;

  template <class input_it>
  vector_of_unique(input_it first, input_it last) {
    _push_back(first, last);
  }

  vector_of_unique(const std::initializer_list<T> &init)
      : vector_of_unique(init.begin(), init.end()) {}

  vector_of_unique(const vector_of_unique &other) { _push_back(other); }

  vector_of_unique(vector_of_unique &&other) NOEXCEPT_CXX17 {
    std::swap(vector_, other.vector_);
    std::swap(set_, other.set_);
  }

  vector_of_unique &operator=(const vector_of_unique &other) = default;
  vector_of_unique &operator=(vector_of_unique &&other) = default;
  vector_of_unique &operator=(std::initializer_list<T> ilist) {
    vector_of_unique temp(ilist);
    std::swap(vector_, temp.vector_);
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
  const_reference at(size_type pos) const { return vector_.at(pos); }
  const_reference front() const { return vector_.front(); }
  const_reference operator[](size_type pos) const { return vector_[pos]; }
  const_reference back() const { return vector_.back(); }

  // Iterators
  const_iterator cbegin() const noexcept { return vector_.cbegin(); }
  const_iterator cend() const noexcept { return vector_.cend(); }

  const_reverse_iterator crbegin() const noexcept { return vector_.crbegin(); }
  const_reverse_iterator crend() const noexcept { return vector_.crend(); }

  // Modifiers
  void clear() noexcept {
    vector_.clear();
    set_.clear();
  }

  const_iterator erase(const_iterator pos) {
    if (vector_.empty()) {
      return vector_.cend();
    }
    set_.erase(*pos);
    return vector_.erase(pos);
  }

  const_iterator erase(const_iterator first, const_iterator last) {
    if (first == last) {
      return last;
    }

    for (auto it = first; it != last; ++it) {
      set_.erase(*it);
    }

    return vector_.erase(first, last);
  }

  std::pair<const_iterator, bool> insert(const_iterator pos, const T &value) {
    if (set_.insert(value).second) {
      return std::make_pair(vector_.insert(pos, value), true);
    }
    return std::make_pair(pos, false);
  }

  std::pair<const_iterator, bool> insert(const_iterator pos, T &&value) {
    if (set_.insert(value).second) {
      return std::make_pair(vector_.insert(pos, std::move(value)), true);
    }
    return std::make_pair(pos, false);
  }

  template <class input_it>
  const_iterator insert(const_iterator pos, input_it first, input_it last) {
    auto pos_index = pos - vector_.cbegin();
    auto first_inserted_index = pos_index;
    auto temp_pos = vector_.begin() + (pos - vector_.cbegin());
    auto any_inserted = false;

    for (auto it = first; it != last; ++it) {
      if (set_.insert(*it).second) {
        temp_pos = vector_.insert(temp_pos, *it);
        if (!any_inserted) {
          first_inserted_index = temp_pos - vector_.cbegin();
          any_inserted = true;
        }
        ++temp_pos;
      }
    }
    return any_inserted ? first_inserted_index + vector_.cbegin()
                        : pos_index + vector_.cbegin();
  }

  const_iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  template <class... Args>
  std::pair<const_iterator, bool> emplace(const_iterator pos, Args &&...args) {
    if (set_.emplace(args...).second) {
      return std::make_pair(vector_.emplace(pos, std::forward<Args>(args)...),
                            true);
    }
    return std::make_pair(pos, false);
  }

#if __cplusplus < 201703L
  template <class... Args>
  void emplace_back(Args &&...args) {
    if (set_.emplace(args...).second) {
      vector_.emplace_back(std::forward<Args>(args)...);
    }
  }
#else
  template <class... Args>
  std::optional<std::reference_wrapper<T>> emplace_back(Args &&...args) {
    if (set_.emplace(args...).second) {
      return vector_.emplace_back(std::forward<Args>(args)...);
    }
    return std::nullopt;
  }
#endif

  void pop_back() {
    if (!vector_.empty()) {
      const auto &f = vector_.back();
      vector_.pop_back();
      set_.erase(f);
    }
  }

  bool push_back(const T &value) {
    if (set_.insert(value).second) {
      vector_.push_back(value);
      return true;
    }
    return false;
  }

  bool push_back(T &&value) {
    auto temp = std::move(value);
    if (set_.insert(temp).second) {
      vector_.push_back(std::move(temp));
      return true;
    }
    return false;
  }

  template <class input_it>
  void _push_back(input_it first, input_it last) {
    while (first != last) {
      push_back(*first++);
    }
  }

  bool _push_back(const vector_of_unique<T, Hash> &other) {
    return _push_back(other.vector_);
  }

  bool _push_back(const std::vector<T> &other) {
    bool any_added = false;
    for (const auto &entry : other) {
      auto added = push_back(entry);
      any_added = any_added || added;
    }
    return any_added;
  }

  void swap(vector_of_unique &other) NOEXCEPT_CXX17 {
    vector_.swap(other.vector_);
    set_.swap(other.set_);
  }

  // Capacity
  bool empty() const noexcept { return vector_.empty(); }

  size_type size() const noexcept { return vector_.size(); }

// Look up
#if __cplusplus < 202002L
  const_iterator find(const T &x) const {
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
  template <class K>
  const_iterator find(const K &x) const {
    auto it = cbegin();
    while (it != cend()) {
      if (*it == x) {
        return it;
      }
      it++;
    }
    return cend();
  }
#endif

#if __cplusplus > 202002L
  bool contains(const key_type &key) const { return set_.contains(key); }

  template <class K>
  bool contains(const K &x) const {
    return set_.contains(x);
  }
#endif

  // Destructor
  ~vector_of_unique() = default;

  // Get member variables
  const VectorType &vector() const { return vector_; }
  const UnorderedSetType &set() const { return set_; }

 private:
  VectorType vector_;
  UnorderedSetType set_;
};  // class vector_of_unique

// Non-member function
#if __cplusplus >= 202002L && __cplusplus < 202600L
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class U>
typename vector_of_unique<T, Hash, KeyEqual>::size_type erase(
    vector_of_unique<T, Hash, KeyEqual> &c, const U &value) {
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
typename vector_of_unique<T, Hash, KeyEqual>::size_type erase(
    vector_of_unique<T, Hash, KeyEqual> &c, const U &value) {
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
typename vector_of_unique<T, Hash, KeyEqual>::size_type erase_if(
    vector_of_unique<T, Hash, KeyEqual> &c, Pred pred) {
  auto it = c.cbegin();
  typename vector_of_unique<T, Hash, KeyEqual>::size_type r = 0;
  while (it != c.cend()) {
    auto find_it = std::find_if(it, c.cend(), pred);
    if (find_it != c.cend()) {
      it = c.erase(find_it);
      ++r;
    } else {
      ++it;
    }
  }
  return r;
}

// Operators
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator==(const vector_of_unique<T, Hash, KeyEqual> &lhs,
                const vector_of_unique<T, Hash, KeyEqual> &rhs) {
  return (lhs.vector() == rhs.vector());
}

#if __cplusplus < 202002L
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator!=(const vector_of_unique<T, Hash, KeyEqual> &lhs,
                const vector_of_unique<T, Hash, KeyEqual> &rhs) {
  return (lhs.vector() != rhs.vector());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator<(const vector_of_unique<T, Hash, KeyEqual> &lhs,
               const vector_of_unique<T, Hash, KeyEqual> &rhs) {
  return (lhs.vector() < rhs.vector());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator<=(const vector_of_unique<T, Hash, KeyEqual> &lhs,
                const vector_of_unique<T, Hash, KeyEqual> &rhs) {
  return (lhs.vector() <= rhs.vector());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator>(const vector_of_unique<T, Hash, KeyEqual> &lhs,
               const vector_of_unique<T, Hash, KeyEqual> &rhs) {
  return (lhs.vector() > rhs.vector());
}

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
bool operator>=(const vector_of_unique<T, Hash, KeyEqual> &lhs,
                const vector_of_unique<T, Hash, KeyEqual> &rhs) {
  return (lhs.vector() >= rhs.vector());
}
#else
template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
auto operator<=>(const vector_of_unique<T, Hash, KeyEqual> &lhs,
                 const vector_of_unique<T, Hash, KeyEqual> &rhs) {
  return (lhs.vector() <=> rhs.vector());
}
#endif
};  // namespace containerofunique