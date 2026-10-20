# 🧩 Sudoku Solver | CodeAlpha Internship

A Sudoku Solver project developed as part of the **CodeAlpha C++ Programming Internship**. This project aims to solve Sudoku puzzles using C++ and the backtracking algorithm, with an interactive and user-friendly interface built using HTML and CSS.

The project focuses on applying core programming concepts, problem-solving techniques, recursion, and algorithm design to a practical application.

## 📌 Project Overview

Sudoku is a number-placement puzzle consisting of a 9 × 9 grid. The objective is to fill every empty cell with a number from 1 to 9 while following the Sudoku rules.

This project uses C++ to implement the Sudoku-solving logic and HTML/CSS to design the user interface. The frontend and backend will be connected so that users can interact with the puzzle through a browser.

**Project Name:** Sudoku Solver
**Internship:** CodeAlpha C++ Programming Internship
**Project Task:** Task 3 – Sudoku Solver
**Project Status:** In Development

## 🎯 Objectives

* Develop a Sudoku solver using C++.
* Understand and implement the backtracking algorithm.
* Apply recursion to solve Sudoku puzzles.
* Represent the Sudoku board using a two-dimensional array.
* Validate numbers according to Sudoku rules.
* Create a clean and interactive interface using HTML and CSS.
* Improve logical thinking, debugging, and problem-solving skills.

## ✨ Planned Features

* **9 × 9 Sudoku Grid:** Display the complete puzzle board.
* **Interactive Interface:** Allow users to select cells and enter numbers.
* **Sudoku Validation:** Check row, column, and 3 × 3 subgrid constraints.
* **Automatic Solver:** Solve valid Sudoku puzzles using backtracking.
* **New Game:** Start a new puzzle.
* **Reset Puzzle:** Restore the initial puzzle.
* **Check Solution:** Verify whether the entered solution is correct.
* **Difficulty Levels:** Support Easy, Medium, and Hard puzzles.
* **Hint System:** Help users when they get stuck.
* **Responsive Design:** Make the interface usable on desktop and mobile screens.

*Note: Features will be implemented and marked complete as development progresses.*

## 🛠️ Technologies Used

| Technology                   | Purpose                                                  |
| ---------------------------- | -------------------------------------------------------- |
| C++                          | Sudoku-solving algorithm and game logic                  |
| HTML5                        | Structure of the game interface                          |
| CSS3                         | Styling, layout, and responsive design                   |
| JavaScript (if used)         | Frontend interactions and communication with the backend |
| C++ HTTP framework (if used) | Connecting the browser interface to the C++ backend      |

The Sudoku-solving algorithm will remain in C++. HTML and CSS will handle the presentation layer. A suitable communication mechanism will be required to connect the browser with the C++ application.

## 🧠 Core Concepts Used

### 1. Two-Dimensional Arrays

The Sudoku board is represented using a 9 × 9 array.

```cpp
int board[9][9];
```

Each element represents a cell. A value of `0` can represent an empty cell.

### 2. Backtracking Algorithm

Backtracking is used to solve the puzzle by trying possible numbers in empty cells.

The algorithm:

1. Finds an empty cell.
2. Tries numbers from 1 to 9.
3. Checks whether a number is valid.
4. Places the number if it satisfies the rules.
5. Recursively attempts to solve the remaining cells.
6. Backtracks if a choice leads to an invalid arrangement.
7. Continues until the puzzle is solved or no solution exists.

### 3. Recursion

The solving function calls itself to process the remaining empty cells until a solution is found or all possibilities have been explored.

### 4. Constraint Validation

Before placing a number, the program checks:

* The number does not already exist in the same row.
* The number does not already exist in the same column.
* The number does not already exist in the same 3 × 3 subgrid.

## ⚙️ How the Project Works

1. The user opens the Sudoku game in a browser.
2. A Sudoku puzzle is displayed on the screen.
3. The user can enter numbers into empty cells.
4. When the user requests a solution, the puzzle data is passed to the C++ solving logic.
5. The C++ program validates the board and runs the backtracking algorithm.
6. The result is returned to the interface.
7. The solved board or an appropriate error message is displayed.

## 📂 Project Structure

```text
CodeAlpha_SudokuSolver/
│
├── backend/
│   └── main.cpp
│
├── frontend/
│   ├── index.html
│   └── style.css
│
├── README.md
└── .gitignore
```

The structure may be updated as the project develops. Additional files, such as JavaScript files, build configuration, or C++ framework dependencies, may be added if required.

## 🚀 Getting Started

### Prerequisites

* A C++ compiler, such as GCC or MinGW.
* Visual Studio Code or another suitable code editor.
* A modern web browser.
* A configured C++ HTTP framework if the browser interface communicates with a web server.

### Setup

1. Clone or download this repository.
2. Open the project folder in your code editor.
3. Configure the C++ compiler and any required backend dependencies.
4. Implement or build the C++ backend.
5. Open the frontend through the configured local server.
6. Start a Sudoku puzzle and test the solver.

**Important:** Exact compilation and run commands will be added after the backend framework and build configuration have been finalized. Opening the HTML file alone will not execute the C++ solver.

## 🧪 Testing

The project should be tested with the following cases:

* A valid Sudoku puzzle with a solution.
* A puzzle containing empty cells.
* An invalid board with duplicate numbers in a row.
* An invalid board with duplicate numbers in a column.
* An invalid board with duplicate numbers in a 3 × 3 subgrid.
* A puzzle that has no solution.
* A fully completed and correctly solved board.

The solver should distinguish between a solved puzzle, an invalid input, and a valid puzzle that has no solution.

## 📚 Learning Outcomes

Through this project, I aim to improve my understanding of:

* C++ programming fundamentals.
* Two-dimensional arrays.
* Functions and recursion.
* Backtracking algorithms.
* Logical problem-solving.
* Input validation and debugging.
* Basic frontend development with HTML and CSS.
* Integrating a C++ program with a browser-based interface.

## 🔮 Future Improvements

* Generate new Sudoku puzzles with different difficulty levels.
* Add a timer and score tracking.
* Provide hints for selected cells.
* Highlight incorrect entries.
* Add keyboard navigation.
* Improve the responsive design.
* Add automated tests for the solving algorithm.

## 👩‍💻 Internship Project

This project is being developed as part of the **CodeAlpha C++ Programming Internship** to gain practical experience in algorithm implementation and software development.

The goal is to build a working Sudoku Solver, understand its implementation, and document the development process.

## 📄 License

The project can be shared and reused according to the license selected for this repository. A specific license will be added when one is chosen.
