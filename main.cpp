#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <limits>
#include <cctype>
#include <stack>
#include <stdexcept>
#include <sstream>

using namespace std;

struct Difficulty {
    string name;
    int size;
    int boxRows;
    int boxCols;
    int blanks;
};

const Difficulty DIFFICULTIES[] = {
    {"Beginner", 4, 2, 2, 6},
    {"Middle", 6, 2, 3, 17},
    {"Professional", 9, 3, 3, 42}
};

const int DIFFICULTY_COUNT = 3;

class GameException : public runtime_error {
public:
    explicit GameException(const string& message) : runtime_error(message) {}
};

class FileException : public GameException {
public:
    explicit FileException(const string& message) : GameException(message) {}
};

class ValidationException : public GameException {
public:
    explicit ValidationException(const string& message) : GameException(message) {}
};

class User {
protected:
    string name;
    string id;

public:
    User() : name(""), id("") {}
    User(const string& userName, const string& userId) : name(userName), id(userId) {}
    virtual ~User() {}

    string getName() const {
        return name;
    }

    string getId() const {
        return id;
    }

    void setName(const string& userName) {
        name = userName;
    }

    void setId(const string& userId) {
        id = userId;
    }
};

class Player : public User {
private:
    int beginnerWins;
    int middleWins;
    int professionalWins;
    int currentPuzzleLevel;
    int boardSize;
    bool savedPuzzleCounted;
    vector<vector<int> > board;
    vector<vector<int> > original;

public:
    Player() : User(), beginnerWins(0), middleWins(0), professionalWins(0),
               currentPuzzleLevel(0), boardSize(0), savedPuzzleCounted(false) {}

    Player(const string& playerName, const string& playerId)
        : User(playerName, playerId), beginnerWins(0), middleWins(0),
          professionalWins(0), currentPuzzleLevel(0), boardSize(0), savedPuzzleCounted(false) {}

    int getBeginnerWins() const {
        return beginnerWins;
    }

    int getMiddleWins() const {
        return middleWins;
    }

    int getProfessionalWins() const {
        return professionalWins;
    }

    int getCurrentPuzzleLevel() const {
        return currentPuzzleLevel;
    }

    int getBoardSize() const {
        return boardSize;
    }

    bool isSavedPuzzleCounted() const {
        return savedPuzzleCounted;
    }

    const vector<vector<int> >& getBoard() const {
        return board;
    }

    const vector<vector<int> >& getOriginalBoard() const {
        return original;
    }

    void setWins(int beginner, int middle, int professional) {
        beginnerWins = beginner;
        middleWins = middle;
        professionalWins = professional;
    }

    void setGameState(int level, int size, const vector<vector<int> >& current,
                      const vector<vector<int> >& starting, bool counted = false) {
        currentPuzzleLevel = level;
        boardSize = size;
        board = current;
        original = starting;
        savedPuzzleCounted = counted;
    }

    int totalWins() const {
        return beginnerWins + middleWins + professionalWins;
    }

    bool isLevelUnlocked(int index) const {
        if (index == 0) return true;
        if (index == 1) return beginnerWins >= 10;
        if (index == 2) return beginnerWins >= 10 && middleWins >= 50;
        return false;
    }

    void recordWin(int index) {
        if (index == 0) ++beginnerWins;
        else if (index == 1) ++middleWins;
        else if (index == 2) ++professionalWins;
    }

    bool hasSavedBoard() const {
        return !board.empty();
    }
};

class SudokuSolver {
private:
    static mt19937& randomEngine() {
        static mt19937 engine(
            static_cast<unsigned int>(
                chrono::steady_clock::now().time_since_epoch().count()
            )
        );
        return engine;
    }

public:
    static bool isSafe(const vector<vector<int> >& board, int row, int col,
                       int value, int boxRows, int boxCols) {
        int n = static_cast<int>(board.size());

        for (int i = 0; i < n; ++i) {
            if (board[row][i] == value || board[i][col] == value)
                return false;
        }

        int startRow = (row / boxRows) * boxRows;
        int startCol = (col / boxCols) * boxCols;

        for (int r = startRow; r < startRow + boxRows; ++r) {
            for (int c = startCol; c < startCol + boxCols; ++c) {
                if (board[r][c] == value)
                    return false;
            }
        }

        return true;
    }

    static bool solve(vector<vector<int> >& board, int boxRows, int boxCols) {
        int n = static_cast<int>(board.size());

        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (board[r][c] == 0) {
                    vector<int> candidates(n);

                    for (int i = 0; i < n; ++i)
                        candidates[i] = i + 1;

                    shuffle(candidates.begin(), candidates.end(), randomEngine());

                    for (size_t i = 0; i < candidates.size(); ++i) {
                        int value = candidates[i];

                        if (isSafe(board, r, c, value, boxRows, boxCols)) {
                            board[r][c] = value;

                            if (solve(board, boxRows, boxCols))
                                return true;

                            board[r][c] = 0;
                        }
                    }

                    return false;
                }
            }
        }

        return true;
    }

    static bool hasConflict(const vector<vector<int> >& board,
                            const Difficulty& difficulty) {
        int n = difficulty.size;

        if (static_cast<int>(board.size()) != n)
            return true;

        for (int r = 0; r < n; ++r) {
            if (static_cast<int>(board[r].size()) != n)
                return true;

            for (int c = 0; c < n; ++c) {
                int value = board[r][c];

                if (value == 0)
                    continue;

                if (value < 1 || value > n)
                    return true;

                for (int k = 0; k < n; ++k) {
                    if (k != c && board[r][k] == value)
                        return true;
                    if (k != r && board[k][c] == value)
                        return true;
                }

                int startRow = (r / difficulty.boxRows) * difficulty.boxRows;
                int startCol = (c / difficulty.boxCols) * difficulty.boxCols;

                for (int rr = startRow; rr < startRow + difficulty.boxRows; ++rr) {
                    for (int cc = startCol; cc < startCol + difficulty.boxCols; ++cc) {
                        if ((rr != r || cc != c) && board[rr][cc] == value)
                            return true;
                    }
                }
            }
        }

        return false;
    }

    static bool isComplete(const vector<vector<int> >& board) {
        for (size_t r = 0; r < board.size(); ++r) {
            for (size_t c = 0; c < board[r].size(); ++c) {
                if (board[r][c] == 0)
                    return false;
            }
        }
        return true;
    }
};

class PuzzleGenerator {
public:
    virtual ~PuzzleGenerator() {}
    virtual vector<vector<int> > generate(const Difficulty& difficulty) const = 0;
};

class RandomPuzzleGenerator : public PuzzleGenerator {
public:
    vector<vector<int> > generate(const Difficulty& difficulty) const {
        vector<vector<int> > solution(
            difficulty.size,
            vector<int>(difficulty.size, 0)
        );

        if (!SudokuSolver::solve(solution, difficulty.boxRows, difficulty.boxCols))
            throw GameException("Unable to generate a Sudoku solution.");

        vector<int> positions(difficulty.size * difficulty.size);

        for (size_t i = 0; i < positions.size(); ++i)
            positions[i] = static_cast<int>(i);

        static mt19937 engine(
            static_cast<unsigned int>(
                chrono::steady_clock::now().time_since_epoch().count() + 17
            )
        );

        shuffle(positions.begin(), positions.end(), engine);

        int blanks = difficulty.blanks;
        if (blanks > static_cast<int>(positions.size()))
            blanks = static_cast<int>(positions.size());

        for (int i = 0; i < blanks; ++i) {
            int position = positions[i];
            solution[position / difficulty.size][position % difficulty.size] = 0;
        }

        return solution;
    }
};

struct Move {
    int row;
    int col;
    int previousValue;
    int newValue;

    Move() : row(0), col(0), previousValue(0), newValue(0) {}

    Move(int r, int c, int oldValue, int updatedValue)
        : row(r), col(c), previousValue(oldValue), newValue(updatedValue) {}
};

class PlayerRepository {
private:
    static string cleanToken(const string& value) {
        string result;

        for (size_t i = 0; i < value.size(); ++i) {
            unsigned char ch = static_cast<unsigned char>(value[i]);

            if (isalnum(ch) || ch == '_' || ch == '-')
                result += static_cast<char>(ch);
        }

        if (result.empty())
            result = "Player";

        return result;
    }

    static string normalize(const string& value) {
        string result = cleanToken(value);
        for (size_t i = 0; i < result.size(); ++i)
            result[i] = static_cast<char>(tolower(static_cast<unsigned char>(result[i])));
        return result;
    }

    static string fileName(const string& playerName, const string& playerId) {
        return normalize(playerName) + "_" + normalize(playerId) + ".txt";
    }

    static void writeBoard(ofstream& out, const vector<vector<int> >& board) {
        out << board.size() << '\n';

        for (size_t r = 0; r < board.size(); ++r) {
            for (size_t c = 0; c < board[r].size(); ++c)
                out << board[r][c] << ' ';
            out << '\n';
        }
    }

    static bool readBoard(ifstream& in, vector<vector<int> >& board, int boardSize) {
        int rows = 0;

        if (!(in >> rows) || rows < 0 || rows > 9)
            return false;

        if (rows != 0 && rows != boardSize)
            return false;

        board.assign(rows, vector<int>(boardSize, 0));

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < boardSize; ++c) {
                if (!(in >> board[r][c]))
                    return false;

                if (board[r][c] < 0 || board[r][c] > boardSize)
                    return false;
            }
        }

        return true;
    }

public:
    void save(const Player& player) const {
        ofstream out(fileName(player.getName(), player.getId()).c_str());

        if (!out)
            throw FileException("Could not open the player file for saving.");

        out << "SUDOKU_QUEST_SAVE_V3\n";
        out << cleanToken(player.getName()) << '\n';
        out << cleanToken(player.getId()) << '\n';
        out << player.getBeginnerWins() << ' '
            << player.getMiddleWins() << ' '
            << player.getProfessionalWins() << '\n';
        out << player.getCurrentPuzzleLevel() << ' '
            << player.getBoardSize() << ' '
            << (player.isSavedPuzzleCounted() ? 1 : 0) << '\n';

        writeBoard(out, player.getBoard());
        writeBoard(out, player.getOriginalBoard());

        if (!out)
            throw FileException("An error occurred while writing the player file.");
    }

    bool load(const string& playerName, const string& playerId, Player& player) const {
        ifstream in(fileName(playerName, playerId).c_str());

        if (!in)
            return false;

        string header;
        getline(in, header);

        bool version3 = header == "SUDOKU_QUEST_SAVE_V3";
        if (!version3 && header != "SUDOKU_QUEST_SAVE_V2" &&
            header != "SUDOKU_QUEST_SAVE_V1")
            throw FileException("The save file has an unsupported format.");

        string loadedName;
        string loadedId;
        int beginner = 0;
        int middle = 0;
        int professional = 0;
        int levelIndex = 0;
        int boardSize = 0;

        if (!(in >> loadedName >> loadedId))
            throw FileException("The save file is incomplete.");

        if (normalize(loadedName) != normalize(playerName) ||
            normalize(loadedId) != normalize(playerId))
            return false;

        if (!(in >> beginner >> middle >> professional))
            throw FileException("The player progress data is invalid.");

        if (beginner < 0 || middle < 0 || professional < 0)
            throw FileException("The save file contains invalid win counts.");

        int countedValue = 0;
        if (!(in >> levelIndex >> boardSize))
            throw FileException("The saved puzzle information is invalid.");
        if (version3 && !(in >> countedValue))
            throw FileException("The saved puzzle completion status is invalid.");
        if (countedValue != 0 && countedValue != 1)
            throw FileException("The saved puzzle completion status is invalid.");

        if (levelIndex < 0 || levelIndex >= DIFFICULTY_COUNT)
            throw FileException("The saved difficulty level is invalid.");

        if (boardSize != 0 && boardSize != 4 && boardSize != 6 && boardSize != 9)
            throw FileException("The saved board size is invalid.");

        vector<vector<int> > loadedBoard;
        vector<vector<int> > loadedOriginal;

        if (!readBoard(in, loadedBoard, boardSize))
            throw FileException("The saved current board is invalid.");

        if (!readBoard(in, loadedOriginal, boardSize))
            throw FileException("The saved original board is invalid.");

        if ((!loadedBoard.empty() && loadedBoard.size() != static_cast<size_t>(boardSize)) ||
            (!loadedOriginal.empty() && loadedOriginal.size() != static_cast<size_t>(boardSize)))
            throw FileException("The saved board dimensions do not match.");

        Player loaded(loadedName, loadedId);
        loaded.setWins(beginner, middle, professional);
        loaded.setGameState(levelIndex, boardSize, loadedBoard, loadedOriginal, countedValue == 1);
        player = loaded;

        return true;
    }

    bool accountFileExists(const string& playerName, const string& playerId) const {
        ifstream in(fileName(playerName, playerId).c_str());
        return static_cast<bool>(in);
    }
};

class Console {
public:
    static int readInt(const string& prompt, int low, int high) {
        while (true) {
            cout << prompt;
            int value;

            if (cin >> value) {
                if (value >= low && value <= high)
                    return value;
            } else {
                cin.clear();
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a number from "
                 << low << " to " << high << ".\n";
        }
    }

    static string readToken(const string& prompt) {
        string value;
        cout << prompt;

        if (!(cin >> value)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw ValidationException("Unable to read input.");
        }

        return value;
    }

    static void printBoard(const vector<vector<int> >& board,
                           const Difficulty& difficulty) {
        cout << "\n     ";

        for (int c = 0; c < difficulty.size; ++c) {
            if (c > 0 && c % difficulty.boxCols == 0)
                cout << " ";

            cout << setw(2) << c + 1 << " ";
        }

        cout << '\n';

        for (int r = 0; r < difficulty.size; ++r) {
            if (r % difficulty.boxRows == 0) {
                cout << "   +";

                for (int c = 0; c < difficulty.size; ++c) {
                    cout << "---+";

                    if ((c + 1) % difficulty.boxCols == 0 &&
                        c < difficulty.size - 1)
                        cout << "+";
                }

                cout << '\n';
            }

            cout << setw(2) << r + 1 << " |";

            for (int c = 0; c < difficulty.size; ++c) {
                cout << " "
                     << (board[r][c] ? to_string(board[r][c]) : ".")
                     << " |";

                if ((c + 1) % difficulty.boxCols == 0 &&
                    c < difficulty.size - 1)
                    cout << "|";
            }

            cout << '\n';
        }

        cout << "   +";

        for (int c = 0; c < difficulty.size; ++c) {
            cout << "---+";

            if ((c + 1) % difficulty.boxCols == 0 &&
                c < difficulty.size - 1)
                cout << "+";
        }

        cout << "\n\n";
    }
};

class SudokuGame {
private:
    PlayerRepository repository;
    RandomPuzzleGenerator generator;

    static string generatePlayerId() {
        static mt19937 engine(
            static_cast<unsigned int>(
                chrono::steady_clock::now().time_since_epoch().count() + 91
            )
        );

        const string characters = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
        uniform_int_distribution<int> distribution(
            0, static_cast<int>(characters.size()) - 1
        );

        string result = "SQ-";

        for (int i = 0; i < 6; ++i)
            result += characters[distribution(engine)];

        return result;
    }

    Player signUp() {
        string playerName = Console::readToken(
            "\nEnter a one-word name (letters/numbers): "
        );

        Player player(playerName, generatePlayerId());
        repository.save(player);

        cout << "\nAccount created successfully.\n";
        cout << "Name: " << player.getName() << '\n';
        cout << "Player ID: " << player.getId() << '\n';
        cout << "Total Sudoku solved: 0\n";
        cout << "Save this ID for your next sign-in.\n";

        return player;
    }

    bool signIn(Player& player) {
        string playerName = Console::readToken("\nEnter your name: ");
        string playerId = Console::readToken("Enter your Player ID: ");

        if (!repository.load(playerName, playerId, player)) {
            cout << "Account not found. Check your name and ID, or sign up.\n";
            return false;
        }

        cout << "Welcome back, " << player.getName() << "!\n";
        return true;
    }

    void saveState(Player& player, int levelIndex,
                   const vector<vector<int> >& board,
                   const vector<vector<int> >& original, bool counted) {
        player.setGameState(levelIndex, DIFFICULTIES[levelIndex].size, board, original, counted);
        repository.save(player);
    }

    bool isCorrectCompletedBoard(const vector<vector<int> >& board,
                                 const Difficulty& difficulty) const {
        return SudokuSolver::isComplete(board) &&
               !SudokuSolver::hasConflict(board, difficulty);
    }

    void play(Player& player, int levelIndex) {
        if (levelIndex < 0 || levelIndex >= DIFFICULTY_COUNT)
            throw ValidationException("Invalid difficulty selection.");

        const Difficulty& difficulty = DIFFICULTIES[levelIndex];
        vector<vector<int> > board;
        vector<vector<int> > original;
        stack<Move> undoStack;
        stack<Move> redoStack;

        bool canResume =
            player.getCurrentPuzzleLevel() == levelIndex &&
            player.getBoardSize() == difficulty.size &&
            static_cast<int>(player.getBoard().size()) == difficulty.size &&
            static_cast<int>(player.getOriginalBoard().size()) == difficulty.size;

        bool countedThisPuzzle = false;
        if (canResume) {
            board = player.getBoard();
            original = player.getOriginalBoard();
            countedThisPuzzle = player.isSavedPuzzleCounted();
        } else {
            board = generator.generate(difficulty);
            original = board;
        }

        saveState(player, levelIndex, board, original, countedThisPuzzle);

        while (true) {
            cout << "\n=== " << difficulty.name << " Sudoku ("
                 << difficulty.size << "x" << difficulty.size << ") ===\n";
            cout << "Solved in this level: ";
            if (levelIndex == 0) cout << player.getBeginnerWins();
            else if (levelIndex == 1) cout << player.getMiddleWins();
            else cout << player.getProfessionalWins();
            cout << " | Total solved: " << player.totalWins() << '\n';
            Console::printBoard(board, difficulty);

            cout << "1. Enter/change cell\n";
            cout << "2. Check board / submit answer\n";
            cout << "3. Hint\n";
            cout << "4. Solve puzzle\n";
            cout << "5. Reset puzzle\n";
            cout << "6. New puzzle\n";
            cout << "7. Undo last move\n";
            cout << "8. Redo last move\n";
            cout << "9. Save and next puzzle\n";
            cout << "10. Save and return to menu\n";
            cout << "0. Save and exit game\n";

            int action = Console::readInt("Choose: ", 0, 10);

            if (action == 0 || action == 10) {
                saveState(player, levelIndex, board, original, countedThisPuzzle);
                cout << "Puzzle saved. Returning to the player menu.\n";
                return;
            }

            if (action == 1) {
                int row = Console::readInt("Row: ", 1, difficulty.size) - 1;
                int col = Console::readInt("Column: ", 1, difficulty.size) - 1;
                if (original[row][col] != 0) {
                    cout << "This is a fixed clue and cannot be changed.\n";
                    continue;
                }
                int value = Console::readInt(
                    "Value (0 erases, 1-" + to_string(difficulty.size) + "): ",
                    0, difficulty.size);
                int previous = board[row][col];
                if (previous != value) {
                    undoStack.push(Move(row, col, previous, value));
                    while (!redoStack.empty()) redoStack.pop();
                    board[row][col] = value;
                }
            } else if (action == 2) {
                if (SudokuSolver::hasConflict(board, difficulty)) {
                    cout << "Incorrect solution: a number repeats in a row, column, or box. Try again.\n";
                } else if (!SudokuSolver::isComplete(board)) {
                    cout << "Some cells are empty. Please complete the puzzle and check again.\n";
                } else if (!isCorrectCompletedBoard(board, difficulty)) {
                    cout << "The solution is incorrect. Please try again.\n";
                } else {
                    cout << "Congratulations! You have successfully solved the Sudoku!\n";
                    if (!countedThisPuzzle) {
                        player.recordWin(levelIndex);
                        countedThisPuzzle = true;
                        saveState(player, levelIndex, board, original, countedThisPuzzle);
                    } else {
                        cout << "This puzzle was already counted in your score.\n";
                    }
                }
            } else if (action == 3) {
                if (SudokuSolver::hasConflict(board, difficulty)) {
                    cout << "Fix conflicting numbers before asking for a hint.\n";
                    continue;
                }
                vector<vector<int> > solved = board;
                if (!SudokuSolver::solve(solved, difficulty.boxRows, difficulty.boxCols)) {
                    cout << "No solution found for the current entries.\n";
                    continue;
                }
                vector<int> emptyCells;
                for (int r = 0; r < difficulty.size; ++r)
                    for (int c = 0; c < difficulty.size; ++c)
                        if (board[r][c] == 0) emptyCells.push_back(r * difficulty.size + c);
                if (emptyCells.empty()) {
                    cout << "There are no empty cells.\n";
                } else {
                    static mt19937 engine(static_cast<unsigned int>(
                        chrono::steady_clock::now().time_since_epoch().count() + 37));
                    shuffle(emptyCells.begin(), emptyCells.end(), engine);
                    int position = emptyCells[0];
                    int row = position / difficulty.size;
                    int col = position % difficulty.size;
                    undoStack.push(Move(row, col, board[row][col], solved[row][col]));
                    while (!redoStack.empty()) redoStack.pop();
                    board[row][col] = solved[row][col];
                    cout << "Hint: row " << row + 1 << ", column " << col + 1
                         << " = " << solved[row][col] << '\n';
                }
            } else if (action == 4) {
                vector<vector<int> > solved = board;
                if (SudokuSolver::hasConflict(board, difficulty) ||
                    !SudokuSolver::solve(solved, difficulty.boxRows, difficulty.boxCols)) {
                    cout << "The current entries conflict. Please correct them before using Solve.\n";
                    continue;
                }
                board = solved;
                while (!undoStack.empty()) undoStack.pop();
                while (!redoStack.empty()) redoStack.pop();
                cout << "Puzzle solved using recursion and backtracking.\n";
                if (!countedThisPuzzle) {
                    player.recordWin(levelIndex);
                    countedThisPuzzle = true;
                }
                saveState(player, levelIndex, board, original, countedThisPuzzle);
            } else if (action == 5) {
                board = original;
                while (!undoStack.empty()) undoStack.pop();
                while (!redoStack.empty()) redoStack.pop();
                cout << "Puzzle reset. Your solved count has not changed.\n";
            } else if (action == 6) {
                if (isCorrectCompletedBoard(board, difficulty) && !countedThisPuzzle) {
                    player.recordWin(levelIndex);
                    countedThisPuzzle = true;
                    cout << "Correct puzzle recorded before opening a new one.\n";
                } else if (!countedThisPuzzle) {
                    cout << "This puzzle is not correctly completed, so it will not count.\n";
                    cout << "Open a new puzzle anyway? 1. Yes  0. Cancel: ";
                    int confirm = Console::readInt("Choose: ", 0, 1);
                    if (confirm == 0) continue;
                }
                board = generator.generate(difficulty);
                original = board;
                countedThisPuzzle = false;
                while (!undoStack.empty()) undoStack.pop();
                while (!redoStack.empty()) redoStack.pop();
                cout << "New puzzle created. Solve it correctly to increase your count.\n";
            } else if (action == 7) {
                if (undoStack.empty()) {
                    cout << "There are no moves to undo.\n";
                } else {
                    Move move = undoStack.top();
                    undoStack.pop();
                    redoStack.push(move);
                    board[move.row][move.col] = move.previousValue;
                    cout << "Last move undone.\n";
                }
            } else if (action == 8) {
                if (redoStack.empty()) {
                    cout << "There are no moves to redo.\n";
                } else {
                    Move move = redoStack.top();
                    redoStack.pop();
                    undoStack.push(move);
                    board[move.row][move.col] = move.newValue;
                    cout << "Last undone move restored.\n";
                }
            } else if (action == 9) {
                if (!isCorrectCompletedBoard(board, difficulty)) {
                    cout << "Save and next is allowed only after a correct completed solution.\n";
                    continue;
                }
                if (!countedThisPuzzle) {
                    player.recordWin(levelIndex);
                    countedThisPuzzle = true;
                }
                board = generator.generate(difficulty);
                original = board;
                countedThisPuzzle = false;
                while (!undoStack.empty()) undoStack.pop();
                while (!redoStack.empty()) redoStack.pop();
                cout << "Your correct solution was saved. Next puzzle is ready.\n";
            }

            saveState(player, levelIndex, board, original, countedThisPuzzle);
            if (levelIndex == 0 && player.getBeginnerWins() >= 10)
                cout << "Beginner milestone reached! Middle level unlocked.\n";
            if (levelIndex == 1 && player.getMiddleWins() >= 50)
                cout << "Middle milestone reached! Professional level unlocked.\n";
        }
    }

    void showProfile(const Player& player) const {
        cout << "\n========== PLAYER PROFILE ==========\n";
        cout << "Name: " << player.getName() << '\n';
        cout << "Player ID: " << player.getId() << '\n';
        cout << "Game: Sudoku Quest\n";
        cout << "Available levels: Beginner, Middle, Professional\n";
        cout << "====================================\n";
    }

    void showProgress(const Player& player) const {
        cout << "\n========== SCORE & PROGRESS =========\n";
        cout << "Beginner puzzles solved: " << player.getBeginnerWins() << '\n';
        cout << "Middle puzzles solved: " << player.getMiddleWins() << '\n';
        cout << "Professional puzzles solved: " << player.getProfessionalWins() << '\n';
        cout << "Total correctly solved: " << player.totalWins() << '\n';
        cout << "Middle level: " << (player.isLevelUnlocked(1) ? "UNLOCKED" : "LOCKED") << '\n';
        cout << "Professional level: " << (player.isLevelUnlocked(2) ? "UNLOCKED" : "LOCKED") << '\n';
        cout << "=====================================\n";
    }

    void playerMenu(Player& player) {
        while (true) {
            cout << "\n========== PLAYER MENU ==========\n";
            cout << "1. Play Sudoku\n";
            cout << "2. View profile\n";
            cout << "3. View score and progress\n";
            cout << "4. Resume saved puzzle\n";
            cout << "5. Sign out\n";
            cout << "0. Save and exit\n";
            int option = Console::readInt("Choose: ", 0, 5);

            if (option == 0) {
                repository.save(player);
                cout << "Progress saved. Goodbye!\n";
                return;
            }
            if (option == 5) {
                repository.save(player);
                return;
            }
            if (option == 2) {
                showProfile(player);
                continue;
            }
            if (option == 3) {
                showProgress(player);
                continue;
            }
            if (option == 4) {
                if (!player.hasSavedBoard()) {
                    cout << "No saved puzzle is available yet. Choose Play Sudoku first.\n";
                } else {
                    play(player, player.getCurrentPuzzleLevel());
                }
                continue;
            }

            cout << "\nChoose difficulty:\n";
            cout << "1. Beginner (4x4)\n";
            cout << "2. Middle (6x6)\n";
            cout << "3. Professional (9x9)\n";
            cout << "0. Back to player menu\n";
            int levelChoice = Console::readInt("Select level: ", 0, 3);
            if (levelChoice == 0) continue;
            int levelIndex = levelChoice - 1;
            if (!player.isLevelUnlocked(levelIndex)) {
                cout << "This level is locked. Complete the required games first.\n";
                continue;
            }
            play(player, levelIndex);
        }
    }

public:
    void run() {
        cout << "========================================\n";
        cout << "       SUDOKU QUEST - C++ TERMINAL\n";
        cout << "========================================\n";

        while (true) {
            cout << "\n1. Sign Up (new player)\n";
            cout << "2. Sign In (existing player)\n";
            cout << "0. Exit\n";

            int choice = Console::readInt("Choose: ", 0, 2);

            if (choice == 0) {
                cout << "Goodbye!\n";
                return;
            }

            try {
                Player player;

                if (choice == 1) {
                    player = signUp();
                } else {
                    if (!signIn(player))
                        continue;
                }

                playerMenu(player);
            } catch (const GameException& error) {
                cout << "Game error: " << error.what() << '\n';
            } catch (const exception& error) {
                cout << "Unexpected error: " << error.what() << '\n';
            }
        }
    }
};

int main() {
    SudokuGame game;

    try {
        game.run();
    } catch (const exception& error) {
        cout << "Fatal error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
