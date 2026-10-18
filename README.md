# HashMap - a template hash table in C++20

A from-scratch implementation of a hash table with **separate chaining**, written as a header-only template class in modern C++20. Built as a systems-programming exercise: manual memory management, the Rule of Five, custom forward iterators, and STL-compatible interfaces.

![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat&logo=cplusplus&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green)
![Build](https://img.shields.io/badge/build-make-blue)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey)

---

## ✨ Features

- **Header-only** — just `#include "HashMap.hpp"`.
- **Separate chaining** collision resolution with singly linked lists per bucket.
- **Automatic rehashing** — bucket count doubles once the load factor exceeds `0.75`.
- **Full Rule of Five** — copy/move constructors and assignment operators with correct self-assignment handling and moved-from safety.
- **STL-style interface** — `insert`, `insert_or_assign`, `operator[]`, `at`, `erase`, `contains`, `find`, `clear`, `size`, `empty`.
- **Forward iterators** — both `iterator` and `const_iterator`, with conversion from non-const to const, working with range-based `for` and standard algorithms (`std::for_each`, `std::find_if`, `std::count_if`, ...).
- **Exception safety on lookup** — `at()` throws `std::out_of_range` for missing keys.
- **Hashing via `std::hash<Key>`** — works with any key type that has a standard hash specialization and `operator==`.

---

## 📦 Requirements

| Tool | Version |
| --- | --- |
| Compiler | GCC 10+ or Clang 12+ (needs C++20) |
| Build system | GNU Make |
| OS | Linux |

> ⚠️ The build uses `-march=native`, so the resulting binary is tuned to the machine it was compiled on. Remove that flag if you need a portable binary.

---

## 🔨 Build

```bash
git clone https://github.com/xfWeiss/<repo-name>.git
cd <repo-name>
make
```

Build artifacts land in a clean, separated layout:

```
bin/test    # the test executable
obj/*.o     # object files
```

To remove all build artifacts:

```bash
make clean
```

`make clean` performs `rm -rf bin obj`, so no stray object files are left behind.

---

## 🚀 Usage

```cpp
#include "HashMap.hpp"

#include <iostream>
#include <string>

int main() {
    HashMap<std::string, int> map;

    map.insert("one", 1);
    map.insert_or_assign("two", 2);
    map["three"] = 3;

    if (map.contains("two")) {
        std::cout << "two = " << map.at("two") << '\n';
    }

    for (const auto& [key, value] : map) {
        std::cout << key << " = " << value << '\n';
    }

    map.erase("one");
    std::cout << "size = " << map.size() << '\n';
}
```

Compile it directly without Make:

```bash
g++ -std=c++20 -Isrc main.cpp -o app
```

---

## 🧪 Tests

The project ships with a self-contained test suite in `src/main.cpp`, built on `assert` — no external test framework required.

```bash
make
./bin/test
```

Covered scenarios:

- basic operations: `insert`, `insert_or_assign`, `operator[]`, `at`, `erase`, `contains`, `size`, `empty`;
- `std::out_of_range` on missing key via `at()`;
- iteration with explicit iterators and range-based `for`;
- `operator[]` after rehashing (bucket growth under load);
- const iteration and use of a moved-from object;
- copy and move construction/assignment, including copy independence;
- compatibility with standard algorithms.

Expected output ends with:

```
All tests passed successfully!
```

---

## 🗂️ Project structure

```
.
├── src/
│   ├── HashMap.hpp     # the template class (header-only)
│   └── main.cpp        # test suite
├── bin/                # build output: test executable  (generated)
├── obj/                # build output: object files     (generated)
├── Makefile
├── .clang-format
├── .gitignore
├── LICENSE
└── README.md
```

---

## ⚙️ Implementation notes

- **Default bucket count:** `8`. **Max load factor:** `0.75`. When exceeded, the table rehashes to `bucket_count * 2`.
- **Collisions** are resolved by prepending to the bucket's linked list — O(1) insertion at the head.
- **Iteration order** is unspecified and depends on hashing and bucket layout, exactly like `std::unordered_map`.
- **Moved-from objects** are left in a valid empty state (`buckets == nullptr`, `size() == 0`) and remain fully usable — insertions re-initialize the table lazily.
- **Hashing** is fixed to `std::hash<Key>`; a custom hasher is not currently a template parameter.

### Complexity

| Operation | Average | Worst case |
| --- | --- | --- |
| `insert` / `insert_or_assign` | O(1) | O(n) |
| `at` / `operator[]` / `find` | O(1) | O(n) |
| `erase` | O(1) | O(n) |
| `size` / `empty` | O(1) | O(1) |

### Known limitations

- No custom hash function or equality comparator as template parameters.
- No `const` overload of `operator[]` (use `at()` on a const map).
- Not thread-safe.

---
