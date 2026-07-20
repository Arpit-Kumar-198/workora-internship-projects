# Number Guessing Game (C++)

The computer randomly picks a number between 1 and 100. The player has
7 attempts to guess it, receiving "Too High" / "Too Low" feedback after
each guess.

## Files
- `number_guessing_game.cpp` — the full source code

## How to Compile

```
g++ -o game number_guessing_game.cpp
```

## How to Run

Linux/macOS:
```
./game
```

Windows:
```
game.exe
```

## Usage

1. The game picks a secret number between 1 and 100.
2. Enter a guess when prompted.
3. Get feedback: "Too High" or "Too Low".
4. Keep guessing until you find it or run out of 7 attempts.
5. Choose to play again (y/n) at the end.

## Features
- Random number generation seeded with the current time
- Input validation (rejects non-numeric and out-of-range guesses)
- Limited attempts with a remaining-attempts counter
- Replay option without restarting the program
