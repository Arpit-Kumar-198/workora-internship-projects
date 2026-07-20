# Hotel Management System (C++)

A console-based hotel portal that manages rooms, customers, and
bookings — including automatic bill generation at check-out. All data
is stored permanently across three plain-text file repositories, so
everything persists between program runs.

## Files
- `hotel_management_system.cpp` — the full source code
- `rooms.txt`, `customers.txt`, `bookings.txt` — created automatically
  the first time you add data (not included in this zip)

## How to Compile

```
g++ -std=c++17 -o hms hotel_management_system.cpp
```

## How to Run

Linux/macOS:
```
./hms
```

Windows:
```
hms.exe
```

## Menu

```
1. Add Room
2. Add Customer
3. Book Room (Check-In)
4. Check-Out & Generate Bill
5. View All Rooms
6. View All Customers
7. View All Bookings
8. Search Available Rooms
9. Exit
```

## How It Works

- **Rooms** are keyed by room number (you choose it, e.g. `101`), each
  with a type, nightly rate, and status (`AVAILABLE` / `OCCUPIED`).
- **Customers** get an auto-generated ID (`C001`, `C002`, ...) that
  keeps incrementing correctly across restarts.
- **Booking a room** (check-in) requires an existing customer ID and
  an available room number. It stamps today's date as the check-in
  date, marks the room `OCCUPIED`, and creates an `ACTIVE` booking
  with an auto-generated ID (`BK001`, `BK002`, ...).
- **Checking out** looks up the booking by its ID, calculates the
  number of nights stayed (minimum 1), computes the bill (12% tax on
  top of nights × nightly rate), prints a formatted invoice, marks the
  booking `COMPLETED`, and frees the room back to `AVAILABLE`.
- **Search Available Rooms** does a case-insensitive filter by room
  type, or lists everything available if left blank.

## Storage Format

`rooms.txt`:
```
RoomNumber|Type|PricePerNight|Status
```

`customers.txt`:
```
CustomerID|Name|Phone|Email
```

`bookings.txt`:
```
BookingID|CustomerID|RoomNumber|CheckInDate|CheckOutDate|Nights|TotalBill|Status
```
(`CheckOutDate` is `-`, and `Nights`/`TotalBill` are `0`, until the
guest checks out.)

## Features
- Full room / customer / booking lifecycle management
- Permanent file-based storage across all three repositories
- Auto-incrementing, file-persisted customer and booking IDs
- Real date-based night calculation and itemized billing (subtotal,
  tax, total)
- Room availability automatically toggles on booking and check-out
- Case-insensitive room-type search
