# TreeSet – Ordered Set via Binary Search Tree  
### Caltech CS11 Advanced C++ – Final TreeSet Labs (Parts 1–3)

This repository contains my final solution for the three **TreeSet** labs (Parts 1–3) from the **Caltech CS11 Advanced C++** track. The project implements an **ordered set** data structure backed by a **binary search tree (BST)**, with a clean public interface for set-style operations.

Only the final, fully integrated version (after all three labs) is stored here, but the code reflects the progression:

- **Tree Sets, Part 1** – build a TreeSet class that stores a set of values in a binary search tree and exposes only *set* operations (no direct access to tree internals).
- **Tree Sets, Part 2** – add a proper iterator that can traverse the tree in sorted order, and use it to implement higher-level operations on sets.
- **Tree Sets, Part 3** – refine and “finish” the TreeSet implementation: polish the interface, clean up the code, and improve overall usability in line with the lab requirements.

---

## What TreeSet Provides

The final TreeSet implementation behaves like a minimal, educational version of `std::set`:

- Stores **unique, ordered values**.
- Maintains the **binary search tree invariant** internally.
- Exposes only operations that make sense for mathematical sets, such as:
  - Insert / remove elements
  - Membership tests
  - Queries like size / empty
  - Basic set algebra (union, intersection, difference) as specified by the labs
- Supports **in-order iteration** over the elements via a custom iterator type.

Internally, the implementation uses a classic BST node structure (value + left/right pointers), but that representation is intentionally **hidden behind the TreeSet interface**.

---

## Lab Progression & Key Challenges

### Part 1 – Building an Ordered Set on a BST

Core tasks reflected in the final code:

- Design a **TreeSet** class whose public interface looks like an ordered set, not a tree API.
- Implement **insertion, lookup, and removal** while preserving the BST invariant.
- Hide the implementation details:
  - No direct access to tree nodes from outside the class.
  - Only “set-like” operations are exposed.
- Ensure basic correctness:
  - No duplicate elements.
  - Lookups and modifications work as expected across a variety of shapes of trees.

---

### Part 2 – Iteration over the Tree

The second lab introduces iteration over the ordered set using a dedicated iterator type:

- Implement a **TreeSet iterator** that walks the BST in sorted (in-order) order.
- Store enough internal state in the iterator to:
  - Remember where it is in the tree.
  - Move to the next element with `operator++`.
- Handle special cases correctly:
  - Incrementing from the smallest element through to the end.
  - Detected “end” iterator state.
- Use the iterator to implement higher-level operations on TreeSet, such as:
  - Visiting all elements for printing / debugging.
  - Iterating over two sets to compute set algebra operations in sorted order.

---

### Part 3 – Finishing and Polishing the TreeSet

The third lab focuses on turning TreeSet into a clean, usable component:

- **Code organization and style**:
  - Factor common logic into helper functions.
  - Keep the public interface concise and self-explanatory.
- **Usability improvements**:
  - Make common operations straightforward to call from client code.
  - Provide clear behavior for empty sets, boundary iterators, and corner cases.
- **Correctness and robustness**:
  - Ensure dynamic memory is managed properly (constructors, destructors, copy/assignment if required by the lab).
  - Keep the abstraction barrier intact: users interact with an ordered set, not a tree.

The code in this repository is the **final TreeSet version** after applying these refinements.

---

## Example Usage

Below is a small example of how the final TreeSet can be used from client code (names may vary slightly depending on your final interface):

```cpp
#include <iostream>
#include "treeset.h"

int main() {
    // Default TreeSet uses int with std::less<int> as in the tests
    TreeSet<int> s;

    s.add(7);
    s.add(3);
    s.add(11);

    if (s.contains(7)) {
        std::cout << "7 is in the set\n";
    }

    // In-order traversal (ascending for std::less<int>)
    for (auto it = s.begin(); it != s.end(); ++it) {
        int value = *it;
        std::cout << value << " ";
    }
    std::cout << "\n";

    // Example set union operation
    TreeSet<int> t;
    t.add(3);
    t.add(5);

    TreeSet<int> u = s.plus(t);

    std::cout << "Union: " << u << "\n";

    return 0;
}
```
---

## Building and Running

The labs use a standard `make`-based build system.

    make test

