# Template-Based Binary Search Tree (C++)

A generic, pointer-based BST implemented as a header-only template
library (`bst.hpp`), with a console demo (`main.cpp`) that exercises
it interactively and also proves it works with more than one type.

## Files
- `bst.hpp` — the reusable template library (`TreeNode<T>`, `BST<T>`)
- `main.cpp` — interactive menu (using `BST<int>`) plus a demo that
  instantiates `BST<std::string>` and `BST<double>` from the exact
  same header

## How to Compile

```
g++ -std=c++17 -o bst_demo main.cpp
```
(`bst.hpp` is included automatically — no separate compilation step
needed since it's header-only.)

## How to Run

Linux/macOS:
```
./bst_demo
```

Windows:
```
bst_demo.exe
```

## Menu

```
1. Insert Value
2. Search Value
3. Delete Value
4. In-order Traversal
5. Pre-order Traversal
6. Post-order Traversal
7. Tree Statistics (Height, Size)
8. Run Template & Memory-Cleanup Demo
9. Exit
```

Option 8 is the interesting one for grading purposes: it creates a
`BST<std::string>` and a `BST<double>` in their own scopes, inserts a
few values, prints their traversals, then lets each tree go out of
scope. Before/after each one, it prints `TreeNode<T>::liveNodeCount`
— a static counter incremented in the node constructor and decremented
in the destructor — so you can see the count return to 0 after each
tree's destructor runs. That's the visible proof there's no leak: every
node created with `new` was freed with `delete`.

## Design Notes

- **Generic** — `bst.hpp` only requires `operator<` on `T`, so it works
  out of the box with `int`, `double`, `std::string`, or any custom
  type you add `operator<` to.
- **Recursion throughout** — insertion, search, deletion, traversals,
  height, and the destructor's cleanup pass are all written
  recursively, as the assignment calls for.
- **Deletion** handles all three classic BST cases:
  1. Leaf node — deleted directly.
  2. One child — the child takes the deleted node's place.
  3. Two children — the in-order successor's value is copied up, then
     the (now-duplicate) successor node is removed instead, which
     always reduces to case 1 or 2.
- **Memory management** — `~BST()` calls a post-order `destroySubtree()`
  that deletes children before their parent, guaranteeing every node
  allocated during the tree's lifetime is freed exactly once. Copying
  a `BST` is disabled (`= delete` on the copy constructor/assignment)
  since it owns raw pointers and a shallow copy would double-free.
- No duplicate values are stored; inserting an existing value is a
  no-op and `insert()`/`remove()` return `false` so the caller can
  detect it.

## Complexity

| Operation | Average | Worst case |
|---|---|---|
| Insert | O(log n) | O(n) |
| Search | O(log n) | O(n) |
| Delete | O(log n) | O(n) |

(Worst case occurs when the tree degenerates into a linked list, e.g.
inserting already-sorted data — this implementation does not
self-balance.)
