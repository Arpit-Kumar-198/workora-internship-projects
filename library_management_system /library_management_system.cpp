/*
 * Library Management System
 * ----------------------------
 * A console-based library portal demonstrating:
 *   - Class hierarchies & inheritance:
 *       Person      -> Student
 *       LibraryItem -> Book
 *   - Polymorphism via virtual functions (display())
 *   - Dynamic memory management (new/delete, RAII cleanup in destructors)
 *   - Persistent storage using file streams (ifstream/ofstream) to three
 *     "repository" files: books.txt, students.txt, loans.txt
 *
 * Menu:
 *   1. Add Book            5. View All Books
 *   2. Add Student         6. View All Students
 *   3. Issue Book (Loan)   7. View All Loans
 *   4. Return Book         8. Search Book
 *                          9. Exit
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <iomanip>

using namespace std;

const string BOOKS_FILE    = "books.txt";
const string STUDENTS_FILE = "students.txt";
const string LOANS_FILE    = "loans.txt";
const int    LOAN_PERIOD_DAYS = 14;

/* =========================================================
 *                     UTILITY FUNCTIONS
 * ========================================================= */

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string getLine(const string &prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
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

int getPositiveInt(const string &prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value) || value <= 0) {
        cout << "Please enter a positive whole number: ";
        clearInputBuffer();
    }
    clearInputBuffer();
    return value;
}

string toLowerStr(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Splits a '|'-delimited line into its fields
vector<string> splitLine(const string &line, char delim = '|') {
    vector<string> fields;
    stringstream ss(line);
    string field;
    while (getline(ss, field, delim)) {
        fields.push_back(field);
    }
    return fields;
}

string formatTm(tm *t) {
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return string(buf);
}

string getTodayDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return formatTm(ltm);
}

// Adds `days` to a YYYY-MM-DD date string and returns the new date string
string addDaysToDate(const string &dateStr, int days) {
    tm t = {};
    int y, m, d;
    sscanf(dateStr.c_str(), "%d-%d-%d", &y, &m, &d);
    t.tm_year = y - 1900;
    t.tm_mon  = m - 1;
    t.tm_mday = d;
    t.tm_hour = 12; // noon avoids DST edge-case shifting the day
    time_t tt = mktime(&t);
    tt += static_cast<time_t>(days) * 24 * 60 * 60;
    tm *newT = localtime(&tt);
    return formatTm(newT);
}

// Generates an ID like "B001", "S007", "L023"
string generateId(char prefix, int number) {
    stringstream ss;
    ss << prefix << setw(3) << setfill('0') << number;
    return ss.str();
}

// Extracts the numeric part of an ID like "B012" -> 12
int extractIdNumber(const string &id) {
    if (id.size() < 2) return 0;
    try {
        return stoi(id.substr(1));
    } catch (...) {
        return 0;
    }
}

/* =========================================================
 *                     CLASS HIERARCHIES
 * ========================================================= */

// ---- Person -> Student (inheritance #1) ----
class Person {
protected:
    string id;
    string name;

public:
    Person(const string &id, const string &name) : id(id), name(name) {}

    string getId() const { return id; }
    string getName() const { return name; }

    virtual void display() const = 0;      // pure virtual -> Person is abstract
    virtual string toFileString() const = 0;

    virtual ~Person() {
        // Virtual destructor ensures derived-class cleanup runs correctly
        // when deleting through a Person* base pointer.
    }
};

class Student : public Person {
private:
    string department;

public:
    Student(const string &id, const string &name, const string &department)
        : Person(id, name), department(department) {}

    string getDepartment() const { return department; }

    void display() const override {
        cout << "  [" << id << "] " << name
             << " | Department: " << department << "\n";
    }

    string toFileString() const override {
        return id + "|" + name + "|" + department;
    }

    ~Student() override {}
};

// ---- LibraryItem -> Book (inheritance #2) ----
class LibraryItem {
protected:
    string id;
    string title;
    string author;
    int totalCopies;
    int availableCopies;

public:
    LibraryItem(const string &id, const string &title, const string &author,
                int totalCopies, int availableCopies)
        : id(id), title(title), author(author),
          totalCopies(totalCopies), availableCopies(availableCopies) {}

    string getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    int getTotalCopies() const { return totalCopies; }
    int getAvailableCopies() const { return availableCopies; }

    bool checkOut() {
        if (availableCopies <= 0) return false;
        availableCopies--;
        return true;
    }

    void checkIn() {
        if (availableCopies < totalCopies) availableCopies++;
    }

    virtual void display() const = 0;      // pure virtual -> LibraryItem is abstract
    virtual string toFileString() const = 0;

    virtual ~LibraryItem() {}
};

class Book : public LibraryItem {
private:
    string genre;
    string isbn;

public:
    Book(const string &id, const string &title, const string &author,
         const string &genre, const string &isbn,
         int totalCopies, int availableCopies)
        : LibraryItem(id, title, author, totalCopies, availableCopies),
          genre(genre), isbn(isbn) {}

    string getGenre() const { return genre; }
    string getIsbn() const { return isbn; }

    void display() const override {
        cout << "  [" << id << "] \"" << title << "\" by " << author
             << " | Genre: " << genre << " | ISBN: " << isbn
             << " | Copies: " << availableCopies << "/" << totalCopies << "\n";
    }

    string toFileString() const override {
        return id + "|" + title + "|" + author + "|" + genre + "|" + isbn + "|" +
               to_string(totalCopies) + "|" + to_string(availableCopies);
    }

    ~Book() override {}
};

// ---- Loan (plain data record linking a Student and a Book by ID) ----
struct Loan {
    string loanId;
    string studentId;
    string bookId;
    string issueDate;
    string dueDate;
    string returnDate; // "-" while still active
    string status;     // "ACTIVE" or "RETURNED"

    string toFileString() const {
        return loanId + "|" + studentId + "|" + bookId + "|" + issueDate + "|" +
               dueDate + "|" + returnDate + "|" + status;
    }
};

/* =========================================================
 *                    LIBRARY (MANAGER CLASS)
 * ========================================================= */

class Library {
private:
    vector<Book*> books;        // dynamically allocated -> owned by Library
    vector<Student*> students;  // dynamically allocated -> owned by Library
    vector<Loan> loans;         // plain records, stored by value

    int nextBookNum;
    int nextStudentNum;
    int nextLoanNum;

    void loadBooks() {
        ifstream inFile(BOOKS_FILE);
        if (!inFile) return;

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 7) continue;

            Book *b = new Book(f[0], f[1], f[2], f[3], f[4], stoi(f[5]), stoi(f[6]));
            books.push_back(b);
            nextBookNum = max(nextBookNum, extractIdNumber(f[0]) + 1);
        }
    }

    void loadStudents() {
        ifstream inFile(STUDENTS_FILE);
        if (!inFile) return;

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 3) continue;

            Student *s = new Student(f[0], f[1], f[2]);
            students.push_back(s);
            nextStudentNum = max(nextStudentNum, extractIdNumber(f[0]) + 1);
        }
    }

    void loadLoans() {
        ifstream inFile(LOANS_FILE);
        if (!inFile) return;

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 7) continue;

            Loan l{f[0], f[1], f[2], f[3], f[4], f[5], f[6]};
            loans.push_back(l);
            nextLoanNum = max(nextLoanNum, extractIdNumber(f[0]) + 1);
        }
    }

    void saveBooks() const {
        ofstream outFile(BOOKS_FILE, ios::trunc);
        for (const auto *b : books) {
            outFile << b->toFileString() << "\n";
        }
    }

    void saveStudents() const {
        ofstream outFile(STUDENTS_FILE, ios::trunc);
        for (const auto *s : students) {
            outFile << s->toFileString() << "\n";
        }
    }

    void saveLoans() const {
        ofstream outFile(LOANS_FILE, ios::trunc);
        for (const auto &l : loans) {
            outFile << l.toFileString() << "\n";
        }
    }

    Book* findBookById(const string &id) {
        for (auto *b : books) {
            if (b->getId() == id) return b;
        }
        return nullptr;
    }

    Student* findStudentById(const string &id) {
        for (auto *s : students) {
            if (s->getId() == id) return s;
        }
        return nullptr;
    }

public:
    Library() : nextBookNum(1), nextStudentNum(1), nextLoanNum(1) {
        loadBooks();
        loadStudents();
        loadLoans();
    }

    // Rule of three: disable copying since Library owns raw pointers.
    Library(const Library &) = delete;
    Library& operator=(const Library &) = delete;

    ~Library() {
        // Explicit memory management: free every dynamically allocated
        // Book and Student before the Library itself is destroyed.
        for (auto *b : books) delete b;
        for (auto *s : students) delete s;
        books.clear();
        students.clear();
    }

    void addBook() {
        cout << "\n--- Add New Book ---\n";
        string title  = getLine("Title: ");
        string author = getLine("Author: ");
        string genre  = getLine("Genre: ");
        string isbn   = getLine("ISBN: ");
        int copies    = getPositiveInt("Number of copies: ");

        if (title.empty()) {
            cout << "Title cannot be empty. Book not added.\n";
            return;
        }

        string id = generateId('B', nextBookNum++);
        Book *b = new Book(id, title, author, genre, isbn, copies, copies);
        books.push_back(b);
        saveBooks();

        cout << "Book added with ID: " << id << "\n";
    }

    void addStudent() {
        cout << "\n--- Add New Student ---\n";
        string name = getLine("Name: ");
        string dept = getLine("Department: ");

        if (name.empty()) {
            cout << "Name cannot be empty. Student not added.\n";
            return;
        }

        string id = generateId('S', nextStudentNum++);
        Student *s = new Student(id, name, dept);
        students.push_back(s);
        saveStudents();

        cout << "Student added with ID: " << id << "\n";
    }

    void issueBook() {
        cout << "\n--- Issue Book ---\n";
        if (books.empty() || students.empty()) {
            cout << "Need at least one book and one student before issuing a loan.\n";
            return;
        }

        string studentId = getLine("Student ID (e.g. S001): ");
        Student *s = findStudentById(studentId);
        if (!s) {
            cout << "No student found with ID " << studentId << ".\n";
            return;
        }

        string bookId = getLine("Book ID (e.g. B001): ");
        Book *b = findBookById(bookId);
        if (!b) {
            cout << "No book found with ID " << bookId << ".\n";
            return;
        }

        if (!b->checkOut()) {
            cout << "Sorry, \"" << b->getTitle() << "\" has no available copies.\n";
            return;
        }

        Loan loan;
        loan.loanId    = generateId('L', nextLoanNum++);
        loan.studentId = s->getId();
        loan.bookId    = b->getId();
        loan.issueDate = getTodayDate();
        loan.dueDate   = addDaysToDate(loan.issueDate, LOAN_PERIOD_DAYS);
        loan.returnDate = "-";
        loan.status    = "ACTIVE";

        loans.push_back(loan);
        saveBooks();
        saveLoans();

        cout << "Loan created: " << loan.loanId << " | \"" << b->getTitle()
             << "\" issued to " << s->getName() << ". Due back on "
             << loan.dueDate << ".\n";
    }

    void returnBook() {
        cout << "\n--- Return Book ---\n";
        string loanId = getLine("Loan ID (e.g. L001): ");

        for (auto &loan : loans) {
            if (loan.loanId == loanId) {
                if (loan.status == "RETURNED") {
                    cout << "This loan has already been marked as returned.\n";
                    return;
                }

                loan.returnDate = getTodayDate();
                loan.status = "RETURNED";

                Book *b = findBookById(loan.bookId);
                if (b) b->checkIn();

                saveBooks();
                saveLoans();

                cout << "Book returned successfully for loan " << loanId << ".\n";
                return;
            }
        }

        cout << "No loan found with ID " << loanId << ".\n";
    }

    void viewBooks() const {
        cout << "\n--- All Books (" << books.size() << ") ---\n";
        if (books.empty()) {
            cout << "No books in the catalog yet.\n";
            return;
        }
        // Polymorphic display through the LibraryItem base pointer
        for (const LibraryItem *item : books) {
            item->display();
        }
    }

    void viewStudents() const {
        cout << "\n--- All Students (" << students.size() << ") ---\n";
        if (students.empty()) {
            cout << "No students registered yet.\n";
            return;
        }
        // Polymorphic display through the Person base pointer
        for (const Person *p : students) {
            p->display();
        }
    }

    void viewLoans() const {
        cout << "\n--- All Loans (" << loans.size() << ") ---\n";
        if (loans.empty()) {
            cout << "No loans on record yet.\n";
            return;
        }

        string today = getTodayDate();
        for (const auto &loan : loans) {
            cout << "  [" << loan.loanId << "] Student: " << loan.studentId
                 << " | Book: " << loan.bookId
                 << " | Issued: " << loan.issueDate
                 << " | Due: " << loan.dueDate
                 << " | Status: " << loan.status;

            if (loan.status == "ACTIVE" && loan.dueDate < today) {
                cout << " (OVERDUE)";
            }
            if (loan.status == "RETURNED") {
                cout << " | Returned: " << loan.returnDate;
            }
            cout << "\n";
        }
    }

    void searchBook() const {
        string query = getLine("\nEnter title or author (or part of it) to search: ");
        string q = toLowerStr(query);
        bool found = false;

        cout << "\n--- Search Results ---\n";
        for (const auto *b : books) {
            if (toLowerStr(b->getTitle()).find(q) != string::npos ||
                toLowerStr(b->getAuthor()).find(q) != string::npos) {
                b->display();
                found = true;
            }
        }

        if (!found) {
            cout << "No matching books found.\n";
        }
    }
};

/* =========================================================
 *                          MAIN
 * ========================================================= */

void showMenu() {
    cout << "\n-----------------------------------------\n";
    cout << "1. Add Book\n";
    cout << "2. Add Student\n";
    cout << "3. Issue Book (Create Loan)\n";
    cout << "4. Return Book\n";
    cout << "5. View All Books\n";
    cout << "6. View All Students\n";
    cout << "7. View All Loans\n";
    cout << "8. Search Book\n";
    cout << "9. Exit\n";
    cout << "-----------------------------------------\n";
    cout << "Enter your choice (1-9): ";
}

int main() {
    Library library; // loads books.txt, students.txt, loans.txt on construction

    cout << "=========================================\n";
    cout << "         LIBRARY MANAGEMENT SYSTEM\n";
    cout << "=========================================\n";

    int choice;
    do {
        showMenu();
        choice = getChoice();

        switch (choice) {
            case 1: library.addBook(); break;
            case 2: library.addStudent(); break;
            case 3: library.issueBook(); break;
            case 4: library.returnBook(); break;
            case 5: library.viewBooks(); break;
            case 6: library.viewStudents(); break;
            case 7: library.viewLoans(); break;
            case 8: library.searchBook(); break;
            case 9: cout << "\nExiting Library Management System. Goodbye!\n"; break;
            default: cout << "\nInvalid choice! Please select a number between 1 and 9.\n";
        }

    } while (choice != 9);

    return 0; // Library's destructor runs here, freeing all Book/Student memory
}
