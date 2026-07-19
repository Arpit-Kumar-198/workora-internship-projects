/*
 * Menu-Driven Calculator
 * -----------------------
 * A simple console-based calculator that performs basic mathematical
 * operations: Addition, Subtraction, Multiplication, and Division.
 *
 * The program keeps showing the menu in a loop until the user chooses
 * to exit, so multiple calculations can be done in one run.
 */

#include <iostream>
#include <limits>

using namespace std;

// Function declarations
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
bool divide(double a, double b, double &result); // returns false on divide-by-zero

void showMenu();
double getNumber(const string &prompt);
int getChoice();
void clearInputBuffer();

int main() {
    int choice;

    cout << "=========================================\n";
    cout << "        MENU-DRIVEN CALCULATOR\n";
    cout << "=========================================\n";

    do {
        showMenu();
        choice = getChoice();

        if (choice == 5) {
            cout << "\nExiting calculator. Goodbye!\n";
            break;
        }

        if (choice < 1 || choice > 5) {
            cout << "\nInvalid choice! Please select a number between 1 and 5.\n\n";
            continue;
        }

        double num1 = getNumber("Enter first number: ");
        double num2 = getNumber("Enter second number: ");
        double result = 0.0;

        switch (choice) {
            case 1:
                result = add(num1, num2);
                cout << "\nResult: " << num1 << " + " << num2 << " = " << result << "\n\n";
                break;

            case 2:
                result = subtract(num1, num2);
                cout << "\nResult: " << num1 << " - " << num2 << " = " << result << "\n\n";
                break;

            case 3:
                result = multiply(num1, num2);
                cout << "\nResult: " << num1 << " * " << num2 << " = " << result << "\n\n";
                break;

            case 4:
                if (divide(num1, num2, result)) {
                    cout << "\nResult: " << num1 << " / " << num2 << " = " << result << "\n\n";
                } else {
                    cout << "\nError: Division by zero is not allowed!\n\n";
                }
                break;
        }

    } while (true);

    return 0;
}

// Displays the operation menu
void showMenu() {
    cout << "-----------------------------------------\n";
    cout << "1. Addition (+)\n";
    cout << "2. Subtraction (-)\n";
    cout << "3. Multiplication (*)\n";
    cout << "4. Division (/)\n";
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
    return choice;
}

// Reads a valid double from the user with a custom prompt
double getNumber(const string &prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Invalid input! Please enter a valid number: ";
        clearInputBuffer();
    }
    return value;
}

// Clears invalid input from the input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Arithmetic operation functions
double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

bool divide(double a, double b, double &result) {
    if (b == 0) {
        return false;
    }
    result = a / b;
    return true;
}
