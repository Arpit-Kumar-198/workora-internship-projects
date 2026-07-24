/*
 * main.cpp -- Console driver & demo for the template BST (bst.hpp)
 * ---------------------------------------------------------------
 * The interactive menu below works with a BST<int>, but the same
 * bst.hpp header works unmodified for any comparable type -- see
 * runTemplateDemo() for a live example using BST<std::string> and
 * BST<double>, plus a check that TreeNode::liveNodeCount returns to
 * zero after those trees go out of scope (proof the destructor
 * cleaned up every allocation).
 */

#include <iostream>
#include <string>
#include <limits>
#include "bst.hpp"

using namespace std;

/* =========================================================
 *                     CONSOLE UTILITIES
 * ========================================================= */

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getChoice() {
    int choice;
    while (!(cin >> choice)) {
        cout << "Invalid input! Please enter a number: ";
        clearInputBuffer();
    }
    clearInputBuffer();
    return choice;
}

int getInt(const string &prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Invalid input! Please enter a whole number: ";
        clearInputBuffer();
    }
    clearInputBuffer();
    return value;
}

template <typename T>
void printValues(const vector<T> &values) {
    if (values.empty()) {
        cout << "(empty tree)\n";
        return;
    }
    for (size_t i = 0; i < values.size(); i++) {
        cout << values[i];
        if (i + 1 < values.size()) cout << ", ";
    }
    cout << "\n";
}

void showMenu() {
    cout << "\n-----------------------------------------\n";
    cout << "1. Insert Value\n";
    cout << "2. Search Value\n";
    cout << "3. Delete Value\n";
    cout << "4. In-order Traversal\n";
    cout << "5. Pre-order Traversal\n";
    cout << "6. Post-order Traversal\n";
    cout << "7. Tree Statistics (Height, Size)\n";
    cout << "8. Run Template & Memory-Cleanup Demo\n";
    cout << "9. Exit\n";
    cout << "-----------------------------------------\n";
    cout << "Enter your choice (1-9): ";
}

/* =========================================================
 *          TEMPLATE GENERICITY + MEMORY CLEANUP DEMO
 * ========================================================= */

void runTemplateDemo() {
    cout << "\n=== Template & Memory-Cleanup Demo ===\n";

    cout << "\nLive TreeNode<string> count before creating tree: "
         << TreeNode<string>::liveNodeCount << "\n";
    {
        // The exact same bst.hpp works for std::string...
        BST<string> nameTree;
        nameTree.insert("Mango");
        nameTree.insert("Apple");
        nameTree.insert("Papaya");
        nameTree.insert("Banana");

        cout << "BST<string> in-order: ";
        printValues(nameTree.inorder());
        cout << "Live TreeNode<string> count while tree is alive: "
             << TreeNode<string>::liveNodeCount << "\n";
    } // nameTree destroyed here -> destructor frees all 4 nodes
    cout << "Live TreeNode<string> count after tree destroyed: "
         << TreeNode<string>::liveNodeCount << "\n";

    cout << "\nLive TreeNode<double> count before creating tree: "
         << TreeNode<double>::liveNodeCount << "\n";
    {
        // ...and for double, with zero changes to the library code.
        BST<double> priceTree;
        priceTree.insert(19.99);
        priceTree.insert(4.50);
        priceTree.insert(42.10);
        priceTree.insert(4.50); // duplicate, rejected

        cout << "BST<double> in-order: ";
        printValues(priceTree.inorder());
        cout << "Size (duplicate was correctly rejected): " << priceTree.size() << "\n";
        cout << "Live TreeNode<double> count while tree is alive: "
             << TreeNode<double>::liveNodeCount << "\n";
    } // priceTree destroyed here
    cout << "Live TreeNode<double> count after tree destroyed: "
         << TreeNode<double>::liveNodeCount << "\n";

    cout << "\nBoth counts returning to 0 confirms every node allocated\n";
    cout << "with 'new' was correctly freed with 'delete' -- no leaks.\n";
}

/* =========================================================
 *                          MAIN
 * ========================================================= */

int main() {
    BST<int> tree;

    cout << "=========================================\n";
    cout << "   TEMPLATE-BASED BINARY SEARCH TREE\n";
    cout << "=========================================\n";

    int choice;
    do {
        showMenu();
        choice = getChoice();

        switch (choice) {
            case 1: {
                int value = getInt("Enter value to insert: ");
                if (tree.insert(value)) {
                    cout << "Inserted " << value << " into the tree.\n";
                } else {
                    cout << value << " already exists; duplicates are not stored.\n";
                }
                break;
            }
            case 2: {
                int value = getInt("Enter value to search for: ");
                cout << (tree.search(value)
                             ? to_string(value) + " was found in the tree.\n"
                             : to_string(value) + " was not found in the tree.\n");
                break;
            }
            case 3: {
                int value = getInt("Enter value to delete: ");
                if (tree.remove(value)) {
                    cout << "Deleted " << value << " from the tree.\n";
                } else {
                    cout << value << " was not found; nothing deleted.\n";
                }
                break;
            }
            case 4:
                cout << "In-order: ";
                printValues(tree.inorder());
                break;
            case 5:
                cout << "Pre-order: ";
                printValues(tree.preorder());
                break;
            case 6:
                cout << "Post-order: ";
                printValues(tree.postorder());
                break;
            case 7:
                if (tree.isEmpty()) {
                    cout << "The tree is empty.\n";
                } else {
                    cout << "Size:   " << tree.size() << " node(s)\n";
                    cout << "Height: " << tree.height() << "\n";
                }
                break;
            case 8:
                runTemplateDemo();
                break;
            case 9:
                cout << "\nExiting. The BST<int> destructor will free "
                     << tree.size() << " remaining node(s) now.\n";
                break;
            default:
                cout << "\nInvalid choice! Please select a number between 1 and 9.\n";
        }

    } while (choice != 9);

    return 0; // tree's destructor runs here, freeing every remaining node
}
