/*
 * bst.hpp -- Template-based Binary Search Tree
 * -----------------------------------------------
 * A generic, pointer-based BST built entirely with recursion. Works
 * with any type T that supports operator< (int, double, std::string,
 * or any custom type you provide operator< for).
 *
 * Supports:
 *   - Node insertion (no duplicates)
 *   - Search
 *   - Deletion (all three cases: leaf, one child, two children)
 *   - In-order, pre-order, and post-order traversals
 *   - Height / size queries
 *   - A live-node counter (TreeNode<T>::liveNodeCount) so client code
 *     can verify the destructor actually freed every allocated node.
 *
 * Being header-only is intentional: template classes generally need
 * their full definition visible wherever they're instantiated, so this
 * is written to be #include'd directly (the usual pattern for C++
 * template libraries, similar to how much of the STL is distributed).
 */

#ifndef BST_HPP
#define BST_HPP

#include <vector>
#include <algorithm>

template <typename T>
struct TreeNode {
    T data;
    TreeNode *left;
    TreeNode *right;

    explicit TreeNode(const T &value) : data(value), left(nullptr), right(nullptr) {
        ++liveNodeCount;
    }

    ~TreeNode() {
        --liveNodeCount;
    }

    // Tracks how many TreeNode<T> instances currently exist. After a
    // BST<T> is destroyed, this should have dropped back to whatever
    // it was before that tree was created -- a simple, dependency-free
    // way to confirm there's no memory leak.
    static long liveNodeCount;
};

template <typename T>
long TreeNode<T>::liveNodeCount = 0;

template <typename T>
class BST {
private:
    TreeNode<T> *root;
    int nodeCount;

    // ---- Insertion (recursive) ----
    TreeNode<T>* insertRec(TreeNode<T> *node, const T &value, bool &inserted) {
        if (node == nullptr) {
            inserted = true;
            return new TreeNode<T>(value);
        }
        if (value < node->data) {
            node->left = insertRec(node->left, value, inserted);
        } else if (node->data < value) {
            node->right = insertRec(node->right, value, inserted);
        } else {
            inserted = false; // equal value already present; no duplicates stored
        }
        return node;
    }

    // ---- Search (recursive) ----
    bool searchRec(TreeNode<T> *node, const T &value) const {
        if (node == nullptr) return false;
        if (value < node->data) return searchRec(node->left, value);
        if (node->data < value) return searchRec(node->right, value);
        return true;
    }

    TreeNode<T>* findMinNode(TreeNode<T> *node) const {
        while (node->left != nullptr) node = node->left;
        return node;
    }

    // ---- Deletion (recursive, all three classic cases) ----
    TreeNode<T>* removeRec(TreeNode<T> *node, const T &value, bool &removed) {
        if (node == nullptr) {
            removed = false;
            return nullptr;
        }

        if (value < node->data) {
            node->left = removeRec(node->left, value, removed);
        } else if (node->data < value) {
            node->right = removeRec(node->right, value, removed);
        } else {
            removed = true;

            // Case 1: leaf node
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            }
            // Case 2: single child (right only)
            if (node->left == nullptr) {
                TreeNode<T> *rightChild = node->right;
                delete node;
                return rightChild;
            }
            // Case 2: single child (left only)
            if (node->right == nullptr) {
                TreeNode<T> *leftChild = node->left;
                delete node;
                return leftChild;
            }
            // Case 3: two children -> copy in-order successor's value up,
            // then remove that successor node (which has at most one
            // child, so it recurses into case 1 or 2 above).
            TreeNode<T> *successor = findMinNode(node->right);
            node->data = successor->data;
            bool dummy;
            node->right = removeRec(node->right, successor->data, dummy);
        }
        return node;
    }

    // ---- Traversals (recursive) ----
    void inorderRec(TreeNode<T> *node, std::vector<T> &out) const {
        if (!node) return;
        inorderRec(node->left, out);
        out.push_back(node->data);
        inorderRec(node->right, out);
    }

    void preorderRec(TreeNode<T> *node, std::vector<T> &out) const {
        if (!node) return;
        out.push_back(node->data);
        preorderRec(node->left, out);
        preorderRec(node->right, out);
    }

    void postorderRec(TreeNode<T> *node, std::vector<T> &out) const {
        if (!node) return;
        postorderRec(node->left, out);
        postorderRec(node->right, out);
        out.push_back(node->data);
    }

    int heightRec(TreeNode<T> *node) const {
        if (!node) return -1; // empty subtree height; a single node has height 0
        return 1 + std::max(heightRec(node->left), heightRec(node->right));
    }

    // Recursively frees every node in the subtree (post-order: children
    // before parent, so nothing is ever accessed after being freed).
    void destroySubtree(TreeNode<T> *node) {
        if (!node) return;
        destroySubtree(node->left);
        destroySubtree(node->right);
        delete node;
    }

public:
    BST() : root(nullptr), nodeCount(0) {}

    // Rule of three: BST owns raw node pointers, so copying is disabled
    // to prevent double-frees / shallow copies of the tree structure.
    BST(const BST &) = delete;
    BST& operator=(const BST &) = delete;

    ~BST() {
        destroySubtree(root); // frees every node exactly once
        root = nullptr;
    }

    // Returns false if the value already exists (duplicates are rejected).
    bool insert(const T &value) {
        bool inserted = false;
        root = insertRec(root, value, inserted);
        if (inserted) ++nodeCount;
        return inserted;
    }

    bool search(const T &value) const {
        return searchRec(root, value);
    }

    // Returns false if the value wasn't found.
    bool remove(const T &value) {
        bool removed = false;
        root = removeRec(root, value, removed);
        if (removed) --nodeCount;
        return removed;
    }

    std::vector<T> inorder() const {
        std::vector<T> out;
        inorderRec(root, out);
        return out;
    }

    std::vector<T> preorder() const {
        std::vector<T> out;
        preorderRec(root, out);
        return out;
    }

    std::vector<T> postorder() const {
        std::vector<T> out;
        postorderRec(root, out);
        return out;
    }

    int size() const { return nodeCount; }
    int height() const { return heightRec(root); }
    bool isEmpty() const { return root == nullptr; }
};

#endif // BST_HPP
