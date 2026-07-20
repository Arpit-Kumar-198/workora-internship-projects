# Tic-Tac-Toe (C++, Console, 2 Players)

A classic two-player Tic-Tac-Toe game played in the console on a 3x3 grid.
Players take turns entering a cell number (1-9) to place their mark
('X' or 'O'). The game detects rows, columns, and diagonal wins, as well
as draws, and prevents playing on an already-occupied cell.

## Files
- `tic_tac_toe.cpp` — the full source code

## How to Compile

```
g++ -o ttt tic_tac_toe.cpp
```

## How to Run

Linux/macOS:
```
./ttt
```

Windows:
```
ttt.exe
```

## Usage

1. Player 1 is 'X', Player 2 is 'O'. X always goes first.
2. Cells are numbered 1-9 as shown in the reference grid printed at the
   start of each game:
   ```
    1 | 2 | 3
   -----------
    4 | 5 | 6
   -----------
    7 | 8 | 9
   ```
3. On your turn, type the cell number where you want to place your mark.
4. The game announces a winner as soon as 3 marks line up (row, column,
   or diagonal), or a draw if the board fills up with no winner.
5. Choose to play again (y/n) at the end of each game.

## Features
- Input validation (rejects non-numeric and out-of-range input)
- Occupied-cell detection (won't overwrite an existing mark)
- Full win detection (rows, columns, both diagonals)
- Draw detection
- Replay option without restarting the program
