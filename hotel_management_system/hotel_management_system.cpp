/*
 * Hotel Management System
 * --------------------------
 * A console-based application that manages:
 *   - Rooms (add, view, availability status)
 *   - Customers (add, view)
 *   - Bookings: check-in and check-out, with automatic bill generation
 *
 * All data is stored permanently in three plain-text file repositories
 * so it persists across program runs:
 *   rooms.txt, customers.txt, bookings.txt
 *
 * Menu:
 *   1. Add Room               6. View All Customers
 *   2. Add Customer           7. View All Bookings
 *   3. Book Room (Check-In)   8. Search Available Rooms
 *   4. Check-Out & Bill       9. Exit
 *   5. View All Rooms
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

const string ROOMS_FILE     = "rooms.txt";
const string CUSTOMERS_FILE = "customers.txt";
const string BOOKINGS_FILE  = "bookings.txt";
const double TAX_RATE       = 0.12; // 12% tax applied to every bill

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

double getPositiveDouble(const string &prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value) || value <= 0) {
        cout << "Please enter a positive number: ";
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

time_t dateStringToTimeT(const string &dateStr) {
    tm t = {};
    int y, m, d;
    sscanf(dateStr.c_str(), "%d-%d-%d", &y, &m, &d);
    t.tm_year = y - 1900;
    t.tm_mon  = m - 1;
    t.tm_mday = d;
    t.tm_hour = 12; // noon avoids DST edge cases
    return mktime(&t);
}

// Number of nights between two YYYY-MM-DD dates (minimum 1, per hotel policy)
int nightsBetween(const string &checkIn, const string &checkOut) {
    time_t t1 = dateStringToTimeT(checkIn);
    time_t t2 = dateStringToTimeT(checkOut);
    double diffDays = difftime(t2, t1) / (60 * 60 * 24);
    int nights = static_cast<int>(diffDays + 0.5); // round to nearest day
    return max(nights, 1);
}

string generateId(const string &prefix, int number, int width) {
    stringstream ss;
    ss << prefix << setw(width) << setfill('0') << number;
    return ss.str();
}

int extractIdNumber(const string &id, int prefixLen) {
    if (static_cast<int>(id.size()) <= prefixLen) return 0;
    try {
        return stoi(id.substr(prefixLen));
    } catch (...) {
        return 0;
    }
}

string formatMoney(double amount) {
    stringstream ss;
    ss << fixed << setprecision(2) << amount;
    return ss.str();
}

/* =========================================================
 *                      DATA STRUCTURES
 * ========================================================= */

struct Room {
    string roomNumber;   // unique key, e.g. "101"
    string type;         // e.g. Single, Double, Suite
    double pricePerNight;
    string status;       // "AVAILABLE" or "OCCUPIED"

    string toFileString() const {
        return roomNumber + "|" + type + "|" + formatMoney(pricePerNight) + "|" + status;
    }
};

struct Customer {
    string customerId;
    string name;
    string phone;
    string email;

    string toFileString() const {
        return customerId + "|" + name + "|" + phone + "|" + email;
    }
};

struct Booking {
    string bookingId;
    string customerId;
    string roomNumber;
    string checkInDate;
    string checkOutDate; // "-" while still active
    int nights;           // 0 while still active
    double totalBill;     // 0 while still active
    string status;        // "ACTIVE" or "COMPLETED"

    string toFileString() const {
        return bookingId + "|" + customerId + "|" + roomNumber + "|" + checkInDate + "|" +
               checkOutDate + "|" + to_string(nights) + "|" + formatMoney(totalBill) + "|" + status;
    }
};

/* =========================================================
 *                HOTEL (MANAGER / REPOSITORY CLASS)
 * ========================================================= */

class Hotel {
private:
    vector<Room> rooms;
    vector<Customer> customers;
    vector<Booking> bookings;

    int nextCustomerNum;
    int nextBookingNum;

    void loadRooms() {
        ifstream inFile(ROOMS_FILE);
        if (!inFile) return;
        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 4) continue;
            rooms.push_back({f[0], f[1], stod(f[2]), f[3]});
        }
    }

    void loadCustomers() {
        ifstream inFile(CUSTOMERS_FILE);
        if (!inFile) return;
        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 4) continue;
            customers.push_back({f[0], f[1], f[2], f[3]});
            nextCustomerNum = max(nextCustomerNum, extractIdNumber(f[0], 1) + 1);
        }
    }

    void loadBookings() {
        ifstream inFile(BOOKINGS_FILE);
        if (!inFile) return;
        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 8) continue;
            Booking b{f[0], f[1], f[2], f[3], f[4], stoi(f[5]), stod(f[6]), f[7]};
            bookings.push_back(b);
            nextBookingNum = max(nextBookingNum, extractIdNumber(f[0], 2) + 1);
        }
    }

    void saveRooms() const {
        ofstream outFile(ROOMS_FILE, ios::trunc);
        for (const auto &r : rooms) outFile << r.toFileString() << "\n";
    }

    void saveCustomers() const {
        ofstream outFile(CUSTOMERS_FILE, ios::trunc);
        for (const auto &c : customers) outFile << c.toFileString() << "\n";
    }

    void saveBookings() const {
        ofstream outFile(BOOKINGS_FILE, ios::trunc);
        for (const auto &b : bookings) outFile << b.toFileString() << "\n";
    }

    Room* findRoom(const string &roomNumber) {
        for (auto &r : rooms) if (r.roomNumber == roomNumber) return &r;
        return nullptr;
    }

    Customer* findCustomer(const string &customerId) {
        for (auto &c : customers) if (c.customerId == customerId) return &c;
        return nullptr;
    }

    Booking* findActiveBookingByRoom(const string &roomNumber) {
        for (auto &b : bookings) {
            if (b.roomNumber == roomNumber && b.status == "ACTIVE") return &b;
        }
        return nullptr;
    }

    Booking* findBookingById(const string &bookingId) {
        for (auto &b : bookings) if (b.bookingId == bookingId) return &b;
        return nullptr;
    }

public:
    Hotel() : nextCustomerNum(1), nextBookingNum(1) {
        loadRooms();
        loadCustomers();
        loadBookings();
    }

    void addRoom() {
        cout << "\n--- Add New Room ---\n";
        string roomNumber = getLine("Room number (e.g. 101): ");

        if (roomNumber.empty()) {
            cout << "Room number cannot be empty. Room not added.\n";
            return;
        }
        if (findRoom(roomNumber) != nullptr) {
            cout << "Room " << roomNumber << " already exists.\n";
            return;
        }

        string type = getLine("Room type (Single/Double/Suite): ");
        double price = getPositiveDouble("Price per night: ");

        rooms.push_back({roomNumber, type, price, "AVAILABLE"});
        saveRooms();

        cout << "Room " << roomNumber << " added successfully.\n";
    }

    void addCustomer() {
        cout << "\n--- Add New Customer ---\n";
        string name = getLine("Name: ");
        if (name.empty()) {
            cout << "Name cannot be empty. Customer not added.\n";
            return;
        }
        string phone = getLine("Phone: ");
        string email = getLine("Email: ");

        string id = generateId("C", nextCustomerNum++, 3);
        customers.push_back({id, name, phone, email});
        saveCustomers();

        cout << "Customer added with ID: " << id << "\n";
    }

    void bookRoom() {
        cout << "\n--- Book Room (Check-In) ---\n";
        if (rooms.empty()) {
            cout << "No rooms exist yet. Add a room first.\n";
            return;
        }
        if (customers.empty()) {
            cout << "No customers exist yet. Add a customer first.\n";
            return;
        }

        string customerId = getLine("Customer ID (e.g. C001): ");
        Customer *cust = findCustomer(customerId);
        if (!cust) {
            cout << "No customer found with ID " << customerId << ".\n";
            return;
        }

        string roomNumber = getLine("Room number to book: ");
        Room *room = findRoom(roomNumber);
        if (!room) {
            cout << "No room found with number " << roomNumber << ".\n";
            return;
        }
        if (room->status != "AVAILABLE") {
            cout << "Room " << roomNumber << " is currently occupied.\n";
            return;
        }

        Booking booking;
        booking.bookingId    = generateId("BK", nextBookingNum++, 3);
        booking.customerId   = cust->customerId;
        booking.roomNumber   = room->roomNumber;
        booking.checkInDate  = getTodayDate();
        booking.checkOutDate = "-";
        booking.nights       = 0;
        booking.totalBill    = 0.0;
        booking.status       = "ACTIVE";

        bookings.push_back(booking);
        room->status = "OCCUPIED";

        saveRooms();
        saveBookings();

        cout << "Booking created: " << booking.bookingId << " | Room " << room->roomNumber
             << " (" << room->type << ") checked in for " << cust->name
             << " on " << booking.checkInDate << ".\n";
    }

    void checkOut() {
        cout << "\n--- Check-Out & Generate Bill ---\n";
        string bookingId = getLine("Booking ID (e.g. BK001): ");
        Booking *booking = findBookingById(bookingId);

        if (!booking) {
            cout << "No booking found with ID " << bookingId << ".\n";
            return;
        }
        if (booking->status == "COMPLETED") {
            cout << "This booking has already been checked out.\n";
            return;
        }

        Room *room = findRoom(booking->roomNumber);
        Customer *cust = findCustomer(booking->customerId);

        booking->checkOutDate = getTodayDate();
        booking->nights = nightsBetween(booking->checkInDate, booking->checkOutDate);

        double rate = room ? room->pricePerNight : 0.0;
        double subtotal = booking->nights * rate;
        double tax = subtotal * TAX_RATE;
        double total = subtotal + tax;

        booking->totalBill = total;
        booking->status = "COMPLETED";

        if (room) room->status = "AVAILABLE";

        saveRooms();
        saveBookings();

        cout << "\n============ INVOICE ============\n";
        cout << "Booking ID:   " << booking->bookingId << "\n";
        cout << "Customer:     " << (cust ? cust->name : booking->customerId) << "\n";
        cout << "Room:         " << booking->roomNumber
             << (room ? " (" + room->type + ")" : "") << "\n";
        cout << "Check-In:     " << booking->checkInDate << "\n";
        cout << "Check-Out:    " << booking->checkOutDate << "\n";
        cout << "Nights:       " << booking->nights << "\n";
        cout << "Rate/Night:   " << formatMoney(rate) << "\n";
        cout << "----------------------------------\n";
        cout << "Subtotal:     " << formatMoney(subtotal) << "\n";
        cout << "Tax (12%):    " << formatMoney(tax) << "\n";
        cout << "TOTAL DUE:    " << formatMoney(total) << "\n";
        cout << "==================================\n";
    }

    void viewRooms() const {
        cout << "\n--- All Rooms (" << rooms.size() << ") ---\n";
        if (rooms.empty()) {
            cout << "No rooms in the system yet.\n";
            return;
        }
        for (const auto &r : rooms) {
            cout << "  [" << r.roomNumber << "] " << r.type
                 << " | $" << formatMoney(r.pricePerNight) << "/night"
                 << " | " << r.status << "\n";
        }
    }

    void viewCustomers() const {
        cout << "\n--- All Customers (" << customers.size() << ") ---\n";
        if (customers.empty()) {
            cout << "No customers registered yet.\n";
            return;
        }
        for (const auto &c : customers) {
            cout << "  [" << c.customerId << "] " << c.name
                 << " | Phone: " << c.phone << " | Email: " << c.email << "\n";
        }
    }

    void viewBookings() const {
        cout << "\n--- All Bookings (" << bookings.size() << ") ---\n";
        if (bookings.empty()) {
            cout << "No bookings on record yet.\n";
            return;
        }
        for (const auto &b : bookings) {
            cout << "  [" << b.bookingId << "] Customer: " << b.customerId
                 << " | Room: " << b.roomNumber
                 << " | In: " << b.checkInDate
                 << " | Out: " << (b.checkOutDate.empty() ? "-" : b.checkOutDate)
                 << " | Status: " << b.status;
            if (b.status == "COMPLETED") {
                cout << " | Bill: $" << formatMoney(b.totalBill);
            }
            cout << "\n";
        }
    }

    void searchAvailableRooms() const {
        string query = getLine("\nEnter room type to search (blank for all available): ");
        string q = toLowerStr(query);
        bool found = false;

        cout << "\n--- Available Rooms ---\n";
        for (const auto &r : rooms) {
            if (r.status != "AVAILABLE") continue;
            if (!q.empty() && toLowerStr(r.type).find(q) == string::npos) continue;

            cout << "  [" << r.roomNumber << "] " << r.type
                 << " | $" << formatMoney(r.pricePerNight) << "/night\n";
            found = true;
        }

        if (!found) {
            cout << "No matching available rooms found.\n";
        }
    }
};

/* =========================================================
 *                          MAIN
 * ========================================================= */

void showMenu() {
    cout << "\n-----------------------------------------\n";
    cout << "1. Add Room\n";
    cout << "2. Add Customer\n";
    cout << "3. Book Room (Check-In)\n";
    cout << "4. Check-Out & Generate Bill\n";
    cout << "5. View All Rooms\n";
    cout << "6. View All Customers\n";
    cout << "7. View All Bookings\n";
    cout << "8. Search Available Rooms\n";
    cout << "9. Exit\n";
    cout << "-----------------------------------------\n";
    cout << "Enter your choice (1-9): ";
}

int main() {
    Hotel hotel; // loads rooms.txt, customers.txt, bookings.txt on construction

    cout << "=========================================\n";
    cout << "         HOTEL MANAGEMENT SYSTEM\n";
    cout << "=========================================\n";

    int choice;
    do {
        showMenu();
        choice = getChoice();

        switch (choice) {
            case 1: hotel.addRoom(); break;
            case 2: hotel.addCustomer(); break;
            case 3: hotel.bookRoom(); break;
            case 4: hotel.checkOut(); break;
            case 5: hotel.viewRooms(); break;
            case 6: hotel.viewCustomers(); break;
            case 7: hotel.viewBookings(); break;
            case 8: hotel.searchAvailableRooms(); break;
            case 9: cout << "\nExiting Hotel Management System. Goodbye!\n"; break;
            default: cout << "\nInvalid choice! Please select a number between 1 and 9.\n";
        }

    } while (choice != 9);

    return 0;
}
