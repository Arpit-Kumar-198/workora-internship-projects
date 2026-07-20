/*
 * Number Guessing Game
 * ---------------------
 * The computer randomly selects a number within a given range.
 * The user repeatedly guesses the number and receives feedback
 * (Too High / Too Low) until they guess correctly or run out of
 * attempts.
 */

#include <iostream>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <limits>

using namespace std;

int getValidatedGuess(int min, int max);
void clearInputBuffer();
bool playRound(int min, int max, int maxAttempts);

int main() {
    srand(static_cast<unsigned int>(time(0))); // seed RNG once

    const int MIN_RANGE = 1;
    const int MAX_RANGE = 100;
    const int MAX_ATTEMPTS = 7;

    char playAgain;

    cout << "=========================================\n";
    cout << "         NUMBER GUESSING GAME\n";
    cout << "=========================================\n";

    do {
        cout << "\nI'm thinking of a number between "
             << MIN_RANGE << " and " << MAX_RANGE << ".\n";
        cout << "You have " << MAX_ATTEMPTS << " attempts to guess it!\n\n";

        playRound(MIN_RANGE, MAX_RANGE, MAX_ATTEMPTS);

        cout << "\nWould you like to play again? (y/n): ";
        cin >> playAgain;
        clearInputBuffer();

    } while (playAgain == 'y' || playAgain == 'Y');

    cout << "\nThanks for playing! Goodbye.\n";
    return 0;
}

// Plays one round of the game. Returns true if the user guessed correctly.
bool playRound(int min, int max, int maxAttempts) {
    int secretNumber = min + rand() % (max - min + 1);
    int attempts = 0;
    bool guessedCorrectly = false;

    while (attempts < maxAttempts) {
        int remaining = maxAttempts - attempts;
        cout << "Attempts remaining: " << remaining << "\n";

        int guess = getValidatedGuess(min, max);
        attempts++;

        if (guess == secretNumber) {
            cout << "\nCongratulations! You guessed it in "
                 << attempts << (attempts == 1 ? " attempt!\n" : " attempts!\n");
            guessedCorrectly = true;
            break;
        } else if (guess < secretNumber) {
            cout << "Too Low! Try again.\n\n";
        } else {
            cout << "Too High! Try again.\n\n";
        }
    }

    if (!guessedCorrectly) {
        cout << "\nOut of attempts! The number was: " << secretNumber << "\n";
    }

    return guessedCorrectly;
}

// Reads a guess from the user, validating that it's a number within range
int getValidatedGuess(int min, int max) {
    int guess;
    cout << "Enter your guess (" << min << "-" << max << "): ";

    while (true) {
        if (!(cin >> guess)) {
            cout << "Invalid input! Please enter a whole number: ";
            clearInputBuffer();
            continue;
        }
        if (guess < min || guess > max) {
            cout << "Out of range! Enter a number between "
                 << min << " and " << max << ": ";
            continue;
        }
        break;
    }
    return guess;
}

// Clears invalid input from the input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
