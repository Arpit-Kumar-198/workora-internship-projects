/*
 * Console Tic-Tac-Toe
 * --------------------
 * Two-player console Tic-Tac-Toe on a 3x3 grid.
 * Players take turns placing 'X' and 'O' by choosing a cell number (1-9)
 * as shown on the reference board. The game detects wins, draws, and
 * prevents moves on already-occupied cells.
 */

#include <iostream>
#include <limits>

using namespace std;

const int SIZE = 3;

void printBoard(const char board[SIZE][SIZE]);
void printReference();
bool makeMove(char board[SIZE][SIZE], int cellNumber, char player);
bool checkWin(const char board[SIZE][SIZE], char player);
bool isBoardFull(const char board[SIZE][SIZE]);
int getValidatedCell();
void clearInputBuffer();
void resetBoard(char board[SIZE][SIZE]);

int main() {
    char board[SIZE][SIZE];
    char playAgain;

    cout << "=========================================\n";
    cout << "           TIC-TAC-TOE (2 Players)\n";
    cout << "=========================================\n";

    do {
        resetBoard(board);
        char currentPlayer = 'X';
        bool gameOver = false;
        int movesMade = 0;

        cout << "\nPlayer 1: X   |   Player 2: O\n";
        cout << "Cells are numbered as shown below:\n";
        printReference();

        while (!gameOver) {
            printBoard(board);
            cout << "Player " << currentPlayer << ", enter your move (1-9): ";

            int cell = getValidatedCell();

            if (!makeMove(board, cell, currentPlayer)) {
                cout << "That cell is already taken! Try again.\n\n";
                continue;
            }

            movesMade++;

            if (checkWin(board, currentPlayer)) {
                printBoard(board);
                cout << "\n*** Player " << currentPlayer << " wins! Congratulations! ***\n";
                gameOver = true;
            } else if (movesMade == SIZE * SIZE) {
                printBoard(board);
                cout << "\nIt's a draw! Well played, both of you.\n";
                gameOver = true;
            } else {
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }

        cout << "\nPlay again? (y/n): ";
        cin >> playAgain;
        clearInputBuffer();

    } while (playAgain == 'y' || playAgain == 'Y');

    cout << "\nThanks for playing Tic-Tac-Toe! Goodbye.\n";
    return 0;
}

// Resets the board to empty cells, using '1'-'9' as placeholders so the
// reference numbers show until a cell is played.
void resetBoard(char board[SIZE][SIZE]) {
    int num = 1;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            board[row][col] = ' ';
            num++;
        }
    }
}

// Prints a small reference grid showing which number maps to which cell
void printReference() {
    cout << "\n 1 | 2 | 3 \n";
    cout << "-----------\n";
    cout << " 4 | 5 | 6 \n";
    cout << "-----------\n";
    cout << " 7 | 8 | 9 \n\n";
}

// Prints the current game board
void printBoard(const char board[SIZE][SIZE]) {
    cout << "\n";
    for (int row = 0; row < SIZE; row++) {
        cout << " ";
        for (int col = 0; col < SIZE; col++) {
            cout << board[row][col];
            if (col < SIZE - 1) cout << " | ";
        }
        cout << "\n";
        if (row < SIZE - 1) cout << "-----------\n";
    }
    cout << "\n";
}

// Converts a cell number (1-9) into row/col and places the player's mark
// if the cell is empty. Returns false if the cell is already occupied.
bool makeMove(char board[SIZE][SIZE], int cellNumber, char player) {
    int index = cellNumber - 1;
    int row = index / SIZE;
    int col = index % SIZE;

    if (board[row][col] != ' ') {
        return false;
    }

    board[row][col] = player;
    return true;
}

// Checks all rows, columns, and diagonals for a win by the given player
bool checkWin(const char board[SIZE][SIZE], char player) {
    // Rows and columns
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
            return true;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player)
            return true;
    }

    // Diagonals
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
        return true;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
        return true;

    return false;
}

// Checks whether the board has no empty cells left
bool isBoardFull(const char board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++)
        for (int col = 0; col < SIZE; col++)
            if (board[row][col] == ' ')
                return false;
    return true;
}

// Reads a validated cell number between 1 and 9
int getValidatedCell() {
    int cell;
    while (true) {
        if (!(cin >> cell)) {
            cout << "Invalid input! Enter a number between 1 and 9: ";
            clearInputBuffer();
            continue;
        }
        if (cell < 1 || cell > 9) {
            cout << "Out of range! Enter a number between 1 and 9: ";
            continue;
        }
        break;
    }
    return cell;
}

// Clears invalid input from the input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
