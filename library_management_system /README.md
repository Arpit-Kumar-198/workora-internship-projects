# Library Management System (C++)

A console-based library portal demonstrating core OOP concepts:
class hierarchies with inheritance, polymorphism, explicit dynamic
memory management, and persistent file-based storage.

## Class Design

| Hierarchy | Base (abstract) | Derived |
|---|---|---|
| People | `Person` (pure virtual `display()`) | `Student` |
| Catalog items | `LibraryItem` (pure virtual `display()`) | `Book` |

- `Loan` is a plain data record linking a `Student` ID and a `Book` ID
  (issue date, due date, return date, status).
- `Library` is the manager class: it owns all `Book*` and `Student*`
  objects (allocated with `new`), loads/saves them via file streams,
  and frees every allocation in its destructor (`~Library()`), so
  there is a single, well-defined place where memory is released.
- Copying `Library` is explicitly disabled (`= delete` on the copy
  constructor and assignment operator) since it manages raw owning
  pointers — this avoids accidental double-frees.
- `viewBooks()` and `viewStudents()` iterate over `LibraryItem*` /
  `Person*` base-class pointers and call the overridden `display()`
  on each one, demonstrating runtime polymorphism.

## Files
- `library_management_system.cpp` — the full source code
- `books.txt`, `students.txt`, `loans.txt` — created automatically the
  first time you add data (not included in this zip)

## How to Compile

```
g++ -std=c++17 -o lms library_management_system.cpp
```

## How to Run

Linux/macOS:
```
./lms
```

Windows:
```
lms.exe
```

## Menu

```
1. Add Book
2. Add Student
3. Issue Book (Create Loan)
4. Return Book
5. View All Books
6. View All Students
7. View All Loans
8. Search Book
9. Exit
```

## Usage Notes

- Book and student IDs are auto-generated (`B001`, `S001`, ...) and
  keep incrementing correctly even after restarting the program,
  since the next number is derived from the highest ID already saved
  to file.
- **Issue Book** checks that the student and book exist and that a
  copy is available, then creates a loan with a 14-day due date from
  today.
- **Return Book** looks up the loan by its Loan ID, marks it returned,
  records the return date, and gives the copy back to the book's
  available count.
- **View All Loans** flags any active loan whose due date has passed
  as `(OVERDUE)`.
- **Search Book** does a case-insensitive partial match on title or
  author.

## Storage Format

`books.txt` — one line per book:
```
BookID|Title|Author|Genre|ISBN|TotalCopies|AvailableCopies
```

`students.txt` — one line per student:
```
StudentID|Name|Department
```

`loans.txt` — one line per loan:
```
LoanID|StudentID|BookID|IssueDate|DueDate|ReturnDate|Status
```
(`ReturnDate` is `-` and `Status` is `ACTIVE` until the book is
returned.)

## Features
- Inheritance: `Person → Student`, `LibraryItem → Book`
- Polymorphism: base-class pointers calling overridden `display()`
- Explicit dynamic memory management with clean, leak-free teardown
- Permanent file-based storage across all three repositories
- Auto-incrementing, file-persisted ID generation
- Real due-date calculation and overdue detection
- Case-insensitive book search
