# 🎮 Sudoku Quest — C++ Terminal Game

Sudoku Quest is a terminal-based Sudoku game developed entirely in C++. It allows players to create accounts, receive a unique Player ID, solve Sudoku puzzles, track their progress, and continue playing from where they left off.

The game includes three difficulty levels, randomized Sudoku puzzles, an automatic hint system, a Sudoku solver, and file handling to save player information and game progress.

## ✨ Features

### 👤 Player Account Management
- **Sign Up:** Create a new player account using your name.
- **Automatic Player ID:** Receive a unique Player ID when creating an account.
- **Sign In:** Access your account using your name and Player ID.
- **Individual Records:** Each player has a separate `.txt` file to store their information and progress.
- **Progress Recovery:** Load your saved progress when you sign in again.

### 🎯 Three Sudoku Levels

| Level | Grid Size | Box Size | Unlock Requirement |
|-------|-----------|----------|--------------------|
| Beginner | 4 × 4 | 2 × 2 | Available from the beginning |
| Middle | 6 × 6 | 2 × 3 | Complete 10 Beginner games |
| Professional | 9 × 9 | 3 × 3 | Complete 50 Middle games after unlocking Middle |

### 🧩 Sudoku Game Features

- **Randomized Puzzles:** Generate Sudoku puzzles with randomized solutions and empty cells.
- **Enter or Change Values:** Fill empty cells with your chosen numbers.
- **Check Board:** Check whether your entries contain duplicate numbers.
- **Hint System:** Get help by filling an empty cell with a valid number.
- **Automatic Solver:** Solve the puzzle using a recursive backtracking algorithm.
- **Reset Puzzle:** Restore the original puzzle.
- **New Puzzle:** Generate a different randomized puzzle.
- **Save and Resume:** Save your current board and continue playing later.
- **Progress Tracking:** View completed games and unlocked levels.

## 🛠️ Technologies Used

- **C++** — Core programming language
- **Standard File Handling** — Reading and writing player records
- **Text Files (`.txt`)** — Storing player details and game progress
- **STL Vectors** — Representing Sudoku grids
- **Random Number Generation** — Generating randomized puzzles
- **Recursion and Backtracking** — Solving Sudoku puzzles

## 📚 C++ Concepts Covered

This project demonstrates the practical use of the following C++ concepts:

- Structures (`struct`)
- Functions and parameter passing
- Conditional statements
- Loops
- Two-dimensional vectors
- File handling using `ifstream` and `ofstream`
- Random number generation
- Recursion
- Backtracking algorithms
- Input validation
- String manipulation
- Sorting and shuffling operations using STL algorithms

## 📂 Project Structure

```text
Sudoku-Quest/
│
├── main.cpp
├── README.md
│
├── Harman_SQ-AB12CD.txt
├── Aman_SQ-CD34EF.txt
└── ...
