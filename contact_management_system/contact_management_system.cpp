/*
 * Contact Management System
 * ---------------------------
 * A file-based console application to Add, View, Search, and Delete
 * contacts. Data is stored permanently in a plain text file
 * ("contacts.txt") so contacts persist between program runs.
 *
 * Storage format (one contact per line, fields separated by '|'):
 *   Name|Phone|Email
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

const string FILE_NAME = "contacts.txt";

struct Contact {
    string name;
    string phone;
    string email;
};

// Function declarations
vector<Contact> loadContacts();
void saveContacts(const vector<Contact> &contacts);
void addContact();
void viewContacts();
void searchContacts();
void deleteContact();
void showMenu();
int getChoice();
string getLine(const string &prompt);
void clearInputBuffer();
string toLower(const string &s);

int main() {
    int choice;

    cout << "=========================================\n";
    cout << "        CONTACT MANAGEMENT SYSTEM\n";
    cout << "=========================================\n";

    do {
        showMenu();
        choice = getChoice();

        switch (choice) {
            case 1:
                addContact();
                break;
            case 2:
                viewContacts();
                break;
            case 3:
                searchContacts();
                break;
            case 4:
                deleteContact();
                break;
            case 5:
                cout << "\nExiting Contact Management System. Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice! Please select a number between 1 and 5.\n";
                break;
        }

    } while (choice != 5);

    return 0;
}

// Displays the main menu
void showMenu() {
    cout << "\n-----------------------------------------\n";
    cout << "1. Add Contact\n";
    cout << "2. View All Contacts\n";
    cout << "3. Search Contact\n";
    cout << "4. Delete Contact\n";
    cout << "5. Exit\n";
    cout << "-----------------------------------------\n";
    cout << "Enter your choice (1-5): ";
}

// Reads and validates the menu choice
int getChoice() {
    int choice;
    while (!(cin >> choice)) {
        cout << "Invalid input! Please enter a number (1-5): ";
        clearInputBuffer();
    }
    clearInputBuffer(); // discard rest of the line (e.g. trailing newline)
    return choice;
}

// Reads a full line of text from the user with a prompt
string getLine(const string &prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

// Clears the input buffer up to and including the next newline
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Converts a string to lowercase (used for case-insensitive search)
string toLower(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Loads all contacts from the storage file into memory
vector<Contact> loadContacts() {
    vector<Contact> contacts;
    ifstream inFile(FILE_NAME);

    if (!inFile) {
        return contacts; // file doesn't exist yet -> no contacts
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string name, phone, email;

        getline(ss, name, '|');
        getline(ss, phone, '|');
        getline(ss, email, '|');

        contacts.push_back({name, phone, email});
    }

    inFile.close();
    return contacts;
}

// Writes the full contact list back to the storage file (overwrites it)
void saveContacts(const vector<Contact> &contacts) {
    ofstream outFile(FILE_NAME, ios::trunc);

    for (const auto &c : contacts) {
        outFile << c.name << "|" << c.phone << "|" << c.email << "\n";
    }

    outFile.close();
}

// Adds a new contact and appends it to the file
void addContact() {
    Contact c;
    cout << "\n--- Add New Contact ---\n";
    c.name = getLine("Enter name: ");

    if (c.name.empty()) {
        cout << "Name cannot be empty. Contact not added.\n";
        return;
    }

    c.phone = getLine("Enter phone number: ");
    c.email = getLine("Enter email: ");

    vector<Contact> contacts = loadContacts();
    contacts.push_back(c);
    saveContacts(contacts);

    cout << "\nContact added successfully!\n";
}

// Displays all contacts currently stored
void viewContacts() {
    vector<Contact> contacts = loadContacts();

    cout << "\n--- All Contacts (" << contacts.size() << ") ---\n";

    if (contacts.empty()) {
        cout << "No contacts found. Add one first!\n";
        return;
    }

    for (size_t i = 0; i < contacts.size(); i++) {
        cout << i + 1 << ". Name: " << contacts[i].name
             << " | Phone: " << contacts[i].phone
             << " | Email: " << contacts[i].email << "\n";
    }
}

// Searches contacts by name (case-insensitive, partial match)
void searchContacts() {
    string query = getLine("\nEnter name (or part of name) to search: ");
    string queryLower = toLower(query);

    vector<Contact> contacts = loadContacts();
    bool found = false;

    cout << "\n--- Search Results ---\n";

    for (size_t i = 0; i < contacts.size(); i++) {
        if (toLower(contacts[i].name).find(queryLower) != string::npos) {
            cout << i + 1 << ". Name: " << contacts[i].name
                 << " | Phone: " << contacts[i].phone
                 << " | Email: " << contacts[i].email << "\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No matching contacts found.\n";
    }
}

// Deletes a contact by displaying a numbered list and asking which to remove
void deleteContact() {
    vector<Contact> contacts = loadContacts();

    cout << "\n--- Delete Contact ---\n";

    if (contacts.empty()) {
        cout << "No contacts to delete.\n";
        return;
    }

    for (size_t i = 0; i < contacts.size(); i++) {
        cout << i + 1 << ". Name: " << contacts[i].name
             << " | Phone: " << contacts[i].phone
             << " | Email: " << contacts[i].email << "\n";
    }

    cout << "Enter the number of the contact to delete (0 to cancel): ";
    int choice;

    while (!(cin >> choice)) {
        cout << "Invalid input! Enter a valid number: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    if (choice == 0) {
        cout << "Deletion cancelled.\n";
        return;
    }

    if (choice < 1 || choice > static_cast<int>(contacts.size())) {
        cout << "Invalid selection. No contact deleted.\n";
        return;
    }

    string removedName = contacts[choice - 1].name;
    contacts.erase(contacts.begin() + (choice - 1));
    saveContacts(contacts);

    cout << "Contact \"" << removedName << "\" deleted successfully.\n";
}
