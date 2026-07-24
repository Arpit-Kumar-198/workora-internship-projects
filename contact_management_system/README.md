# Contact Management System (C++)

A file-based console application to Add, View, Search, and Delete
contacts. Contact data is stored permanently in a plain text file
(`contacts.txt`), which is created automatically in the same folder
the program is run from, so contacts persist between runs.

## Files
- `contact_management_system.cpp` — the full source code
- `contacts.txt` — created automatically the first time you add a
  contact (not included in this zip; the program generates it)

## How to Compile

```
g++ -o cms contact_management_system.cpp
```

## How to Run

Linux/macOS:
```
./cms
```

Windows:
```
cms.exe
```

## Usage

1. **Add Contact** — enter a name, phone number, and email; it's saved
   to `contacts.txt` immediately.
2. **View All Contacts** — lists every stored contact with an index
   number.
3. **Search Contact** — case-insensitive, partial-match search by name.
4. **Delete Contact** — shows a numbered list and removes the contact
   you select.
5. **Exit** — closes the program (all data is already saved to disk).

## Storage Format

Each contact is stored as one line in `contacts.txt`:
```
Name|Phone|Email
```

## Features
- Permanent file-based storage (data survives program restarts)
- Add / View / Search / Delete fully implemented
- Case-insensitive partial-name search
- Input validation and safe handling of empty file / no contacts cases
- Numbered contact list for easy deletion
