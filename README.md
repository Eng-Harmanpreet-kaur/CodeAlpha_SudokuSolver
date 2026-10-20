# Sudoku Quest — C++ Terminal Game (No `<filesystem>`)

This project uses C++ file streams for save/load and does not include `<filesystem>`. Each player's save file is created in the current working directory as `Name_ID.txt`.

## Features
- Sign Up and Sign In with an automatically generated player ID.
- Saves game wins and current Sudoku board in a `.txt` file.
- Resume a saved puzzle after restarting the program.
- Beginner 4×4 (10 wins unlock Middle), Middle 6×6 (50 Middle wins unlock Professional), Professional 9×9.
- Randomized puzzles, Check, Hint, Solve, Reset and New Puzzle.
- Backtracking solver and row/column/box validation.

## Compile on Windows / VS Code
Open a terminal in the folder containing `main.cpp` and run:
```powershell
g++ -std=c++11 -O2 main.cpp -o sudoku.exe
.\sudoku.exe
```
If your compiler supports C++17, you can also use `-std=c++17`.

## Files
- `main.cpp` — all game logic.
- `README.md` — these instructions.

Save files are written next to the executable/current working directory. Keep the `.txt` files if you want to preserve progress.
