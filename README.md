# Container of Unique Entries

A header-only C++ library providing container adaptors that guarantee all elements remain unique, combining the ordered access of standard sequence containers with O(1) average-time duplicate detection.

Full API documentation: https://hjiangcpp.github.io/containerofuniqueentries-doc/

## Containers

### `deque_of_unique`

Combines `std::deque` and `std::unordered_set`. Supports efficient insertion and removal at both ends while rejecting duplicates.

```cpp
#include "dequeofunique.h"

containerofunique::deque_of_unique<int> d = {3, 5, 20, 9};
d.push_front(3);   // duplicate — not added
d.push_back(28);
// d: 3 5 20 9 28
```

### `vector_of_unique`

Combines `std::vector` and `std::unordered_set`. Maintains insertion order with random access while rejecting duplicates.

```cpp
#include "vectorofunique.h"

containerofunique::vector_of_unique<int> v = {1, 2, 3};
v.push_back(2);    // duplicate — not added
v.push_back(4);
// v: 1 2 3 4
```

## Template Parameters

```cpp
template <class T,
          class Hash     = std::hash<T>,
          class KeyEqual = std::equal_to<T>>
class deque_of_unique;

template <class T,
          class Hash     = std::hash<T>,
          class KeyEqual = std::equal_to<T>>
class vector_of_unique;
```

| Parameter  | Description                              | Default             |
|------------|------------------------------------------|---------------------|
| `T`        | Element type (must be hashable)          | —                   |
| `Hash`     | Hash function for the internal set       | `std::hash<T>`      |
| `KeyEqual` | Equality comparator for the internal set | `std::equal_to<T>`  |

## Key Features

- Duplicate elements are silently rejected on insert — no exceptions thrown
- Insertion and lookup are O(1) on average
- Full iterator support (`begin`/`end`, `cbegin`/`cend`, `rbegin`/`rend`)
- Non-member `erase` and `erase_if` (C++20-style free functions available for all supported standards)
- Compatible with C++14, C++17, C++20, and C++23

## API Overview

### Modifiers

| Method | Description |
|--------|-------------|
| `push_back(value)` | Appends if not already present; returns `bool` |
| `push_front(value)` | Prepends if not already present (`deque_of_unique` only); returns `bool` |
| `pop_back()` | Removes the last element |
| `pop_front()` | Removes the first element (`deque_of_unique` only) |
| `insert(pos, value)` | Inserts before `pos` if not a duplicate; returns `{iterator, bool}` |
| `emplace(pos, args...)` | Constructs in-place before `pos` if not a duplicate; returns `{iterator, bool}` |
| `emplace_back(args...)` | Constructs at the end if not a duplicate |
| `erase(pos)` | Removes element at `pos`; returns iterator to next element |
| `erase(first, last)` | Removes elements in range `[first, last)` |
| `clear()` | Removes all elements |
| `assign(first, last)` | Replaces contents with unique elements from range |
| `swap(other)` | Swaps contents with another container |

### Lookup

| Method | Description |
|--------|-------------|
| `find(x)` | Returns iterator to element, or `cend()` if not found |
| `contains(x)` | Returns `bool` (C++20) |

### Non-member Functions

```cpp
// Erase element matching value; returns number of elements removed (0 or 1)
containerofunique::erase(c, value);

// Erase all elements satisfying predicate; returns count removed
containerofunique::erase_if(c, pred);
```

## Requirements

- C++14 or later
- CMake 3.29 or later (for building tests)

## Building and Running Tests

Tests are built with [GoogleTest](https://github.com/google/googletest) and run across C++14, C++17, C++20, and C++23.

```bash
git clone --recurse-submodules <repo-url>
mkdir build && cd build
cmake ..
make
ctest
```

Or run individual test binaries:

```bash
./test_cxx20_vector
./test_cxx20_deque
```