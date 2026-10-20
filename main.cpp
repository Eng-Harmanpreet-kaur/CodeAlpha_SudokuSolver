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

using namespace std;

struct Level {
    string name;
    int n, boxRows, boxCols, blanks;
};

const Level LEVELS[3] = {
    {"Beginner", 4, 2, 2, 6},
    {"Middle", 6, 2, 3, 17},
    {"Professional", 9, 3, 3, 42}
};

struct Player {
    string name, id;
    int beginnerWins, middleWins, professionalWins;
    int currentPuzzleLevel, boardSize;
    vector<vector<int> > board, original;

    Player() : beginnerWins(0), middleWins(0), professionalWins(0),
               currentPuzzleLevel(0), boardSize(0) {}
};

mt19937 rng((unsigned int)chrono::steady_clock::now().time_since_epoch().count());

int totalWins(const Player& p) {
    return p.beginnerWins + p.middleWins + p.professionalWins;
}

bool levelUnlocked(const Player& p, int index) {
    if (index == 0) return true;
    if (index == 1) return p.beginnerWins >= 10;
    return p.beginnerWins >= 10 && p.middleWins >= 50;
}

string cleanName(string name) {
    string result;

    for (size_t i = 0; i < name.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(name[i]);

        if (isalnum(ch) || ch == '_' || ch == '-')
            result += name[i];
    }

    if (result.empty())
        result = "Player";

    return result;
}

string playerFileName(const string& name, const string& id) {
    return cleanName(name) + "_" + cleanName(id) + ".txt";
}

void savePlayer(const Player& p) {
    ofstream out(playerFileName(p.name, p.id).c_str());

    if (!out) {
        cout << "Warning: could not save progress file.\n";
        return;
    }

    out << "SUDOKU_QUEST_SAVE_V1\n";
    out << p.name << '\n' << p.id << '\n';

    out << p.beginnerWins << ' '
        << p.middleWins << ' '
        << p.professionalWins << '\n';

    out << p.currentPuzzleLevel << ' '
        << p.boardSize << '\n';

    out << p.board.size() << '\n';

    for (size_t r = 0; r < p.board.size(); ++r) {
        for (size_t c = 0; c < p.board[r].size(); ++c)
            out << p.board[r][c] << ' ';

        out << '\n';
    }

    out << p.original.size() << '\n';

    for (size_t r = 0; r < p.original.size(); ++r) {
        for (size_t c = 0; c < p.original[r].size(); ++c)
            out << p.original[r][c] << ' ';

        out << '\n';
    }
}

bool loadPlayer(const string& name, const string& id, Player& p) {
    ifstream in(playerFileName(name, id).c_str());

    if (!in)
        return false;

    string header;
    getline(in, header);

    if (header != "SUDOKU_QUEST_SAVE_V1")
        return false;

    Player loaded;

    if (!(in >> loaded.name >> loaded.id))
        return false;

    if (loaded.name != cleanName(name) || loaded.id != cleanName(id))
        return false;

    if (!(in >> loaded.beginnerWins
             >> loaded.middleWins
             >> loaded.professionalWins))
        return false;

    if (!(in >> loaded.currentPuzzleLevel >> loaded.boardSize))
        return false;

    int rows;

    if (!(in >> rows) || rows < 0 || rows > 9)
        return false;

    loaded.board.assign(rows, vector<int>(loaded.boardSize, 0));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < loaded.boardSize; ++c) {
            if (!(in >> loaded.board[r][c]))
                return false;
        }
    }

    if (!(in >> rows) || rows < 0 || rows > 9)
        return false;

    loaded.original.assign(rows, vector<int>(loaded.boardSize, 0));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < loaded.boardSize; ++c) {
            if (!(in >> loaded.original[r][c]))
                return false;
        }
    }

    p = loaded;
    return true;
}

string generateID() {
    const char chars[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

    uniform_int_distribution<int> dist(
        0, (int)sizeof(chars) - 2
    );

    string id = "SQ-";

    for (int i = 0; i < 6; ++i)
        id += chars[dist(rng)];

    return id;
}

bool safeValue(
    const vector<vector<int> >& board,
    int row,
    int col,
    int value,
    int boxRows,
    int boxCols
) {
    int n = (int)board.size();

    for (int i = 0; i < n; ++i) {
        if (board[row][i] == value ||
            board[i][col] == value)
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

bool solveSudoku(
    vector<vector<int> >& board,
    int boxRows,
    int boxCols
) {
    int n = (int)board.size();

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (board[r][c] == 0) {
                vector<int> values(n);

                for (int i = 0; i < n; ++i)
                    values[i] = i + 1;

                shuffle(values.begin(), values.end(), rng);

                for (int value : values) {
                    if (safeValue(
                        board, r, c, value, boxRows, boxCols
                    )) {
                        board[r][c] = value;

                        if (solveSudoku(board, boxRows, boxCols))
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

vector<vector<int> > makePuzzle(const Level& level) {
    vector<vector<int> > solution(
        level.n,
        vector<int>(level.n, 0)
    );

    solveSudoku(solution, level.boxRows, level.boxCols);

    vector<int> positions(level.n * level.n);

    for (int i = 0; i < (int)positions.size(); ++i)
        positions[i] = i;

    shuffle(positions.begin(), positions.end(), rng);

    for (int i = 0; i < level.blanks; ++i) {
        solution[positions[i] / level.n]
                [positions[i] % level.n] = 0;
    }

    return solution;
}

bool hasConflict(
    const vector<vector<int> >& board,
    const Level& level
) {
    int n = level.n;

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            int value = board[r][c];

            if (value == 0)
                continue;

            for (int k = 0; k < n; ++k) {
                if (k != c && board[r][k] == value)
                    return true;

                if (k != r && board[k][c] == value)
                    return true;
            }

            int sr = (r / level.boxRows) * level.boxRows;
            int sc = (c / level.boxCols) * level.boxCols;

            for (int rr = sr; rr < sr + level.boxRows; ++rr) {
                for (int cc = sc; cc < sc + level.boxCols; ++cc) {
                    if ((rr != r || cc != c) &&
                        board[rr][cc] == value)
                        return true;
                }
            }
        }
    }

    return false;
}

bool isComplete(const vector<vector<int> >& board) {
    for (size_t r = 0; r < board.size(); ++r) {
        for (size_t c = 0; c < board[r].size(); ++c) {
            if (board[r][c] == 0)
                return false;
        }
    }

    return true;
}

void printBoard(
    const vector<vector<int> >& board,
    const Level& level
) {
    cout << "\n     ";

    for (int c = 0; c < level.n; ++c) {
        if (c > 0 && c % level.boxCols == 0)
            cout << " ";

        cout << setw(2) << c + 1 << " ";
    }

    cout << '\n';

    for (int r = 0; r < level.n; ++r) {
        if (r % level.boxRows == 0) {
            cout << "   +";

            for (int c = 0; c < level.n; ++c) {
                cout << "---+";

                if ((c + 1) % level.boxCols == 0 &&
                    c < level.n - 1)
                    cout << "+";
            }

            cout << '\n';
        }

        cout << setw(2) << r + 1 << " |";

        for (int c = 0; c < level.n; ++c) {
            cout << " "
                 << (board[r][c] ? to_string(board[r][c]) : ".")
                 << " |";

            if ((c + 1) % level.boxCols == 0 &&
                c < level.n - 1)
                cout << "|";
        }

        cout << '\n';
    }

    cout << "   +";

    for (int c = 0; c < level.n; ++c) {
        cout << "---+";

        if ((c + 1) % level.boxCols == 0 &&
            c < level.n - 1)
            cout << "+";
    }

    cout << "\n\n";
}

int readInt(const string& prompt, int low, int high) {
    while (true) {
        cout << prompt;

        int value;

        if (cin >> value && value >= low && value <= high)
            return value;

        cout << "Invalid input. Enter a number from "
             << low << " to " << high << ".\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void showProgress(const Player& p) {
    cout << "\n========== PLAYER PROGRESS ==========\n";

    cout << "Name: " << p.name << "\n";
    cout << "Player ID: " << p.id << '\n';

    cout << "Beginner wins: " << p.beginnerWins << "/10\n";
    cout << "Middle wins: " << p.middleWins << "/50\n";
    cout << "Professional wins: " << p.professionalWins << '\n';

    cout << "Total wins: " << totalWins(p) << '\n';

    cout << "Middle: "
         << (levelUnlocked(p, 1) ? "UNLOCKED" : "LOCKED")
         << '\n';

    cout << "Professional: "
         << (levelUnlocked(p, 2) ? "UNLOCKED" : "LOCKED")
         << '\n';

    cout << "=====================================\n";
}

void recordWin(Player& p, int levelIndex) {
    if (levelIndex == 0)
        ++p.beginnerWins;
    else if (levelIndex == 1)
        ++p.middleWins;
    else
        ++p.professionalWins;

    savePlayer(p);

    cout << "Progress saved in your .txt file.\n";
}

void playGame(Player& p, int levelIndex) {
    const Level& level = LEVELS[levelIndex];

    vector<vector<int> > board, original;

    if (p.currentPuzzleLevel == levelIndex &&
        p.boardSize == level.n &&
        (int)p.board.size() == level.n &&
        (int)p.original.size() == level.n) {
        board = p.board;
        original = p.original;
    } else {
        board = makePuzzle(level);
        original = board;
    }

    p.currentPuzzleLevel = levelIndex;
    p.boardSize = level.n;
    p.board = board;
    p.original = original;

    savePlayer(p);

    bool countedThisGame = false;

    while (true) {
        cout << "\n=== " << level.name
             << " Sudoku (" << level.n << "x"
             << level.n << ") ===\n";

        printBoard(board, level);

        cout << "1. Enter/change cell\n";
        cout << "2. Check board\n";
        cout << "3. Hint\n";
        cout << "4. Solve\n";
        cout << "5. Reset puzzle\n";
        cout << "6. New puzzle\n";
        cout << "7. Save and return to menu\n";
        cout << "0. Save and exit game\n";

        int action = readInt("Choose: ", 0, 7);

        if (action == 0 || action == 7) {
            p.board = board;
            p.original = original;
            p.currentPuzzleLevel = levelIndex;
            p.boardSize = level.n;

            savePlayer(p);

            cout << "Puzzle saved. You can resume it after signing in again.\n";
            return;
        }

        if (action == 1) {
            int row = readInt("Row: ", 1, level.n) - 1;
            int col = readInt("Column: ", 1, level.n) - 1;

            if (original[row][col] != 0) {
                cout << "This is a fixed clue and cannot be changed.\n";
                continue;
            }

            board[row][col] = readInt(
                "Value (0 erases, 1-" +
                to_string(level.n) + "): ",
                0, level.n
            );
        } else if (action == 2) {
            if (hasConflict(board, level)) {
                cout << "Incorrect: a number repeats in a row, column, or box.\n";
            } else if (!isComplete(board)) {
                cout << "No conflicts found yet, but some cells are empty.\n";
            } else {
                vector<vector<int> > test = board;

                if (solveSudoku(test, level.boxRows, level.boxCols)) {
                    cout << "Congratulations! Sudoku completed correctly.\n";

                    if (!countedThisGame) {
                        recordWin(p, levelIndex);
                        countedThisGame = true;
                    }
                } else {
                    cout << "This board is not solvable.\n";
                }
            }
        } else if (action == 3) {
            if (hasConflict(board, level)) {
                cout << "Fix conflicting numbers before asking for a hint.\n";
                continue;
            }

            vector<vector<int> > solved = board;

            if (!solveSudoku(solved, level.boxRows, level.boxCols)) {
                cout << "No solution found for the current entries.\n";
                continue;
            }

            vector<int> emptyCells;

            for (int r = 0; r < level.n; ++r) {
                for (int c = 0; c < level.n; ++c) {
                    if (board[r][c] == 0)
                        emptyCells.push_back(r * level.n + c);
                }
            }

            if (emptyCells.empty()) {
                cout << "There are no empty cells.\n";
            } else {
                shuffle(emptyCells.begin(), emptyCells.end(), rng);

                int pos = emptyCells[0];
                int r = pos / level.n;
                int c = pos % level.n;

                board[r][c] = solved[r][c];

                cout << "Hint: row " << r + 1
                     << ", column " << c + 1
                     << " = " << solved[r][c] << '\n';
            }
        } else if (action == 4) {
            if (hasConflict(board, level)) {
                cout << "Correct duplicate numbers before solving.\n";
                continue;
            }

            vector<vector<int> > solved = board;

            if (solveSudoku(solved, level.boxRows, level.boxCols)) {
                board = solved;

                cout << "Solved using C++ recursion and backtracking.\n";

                if (!countedThisGame) {
                    recordWin(p, levelIndex);
                    countedThisGame = true;
                }
            } else {
                cout << "No solution found.\n";
            }
        } else if (action == 5) {
            board = original;
            countedThisGame = false;

            cout << "Puzzle reset.\n";
        } else if (action == 6) {
            board = makePuzzle(level);
            original = board;
            countedThisGame = false;

            cout << "New randomized puzzle created.\n";
        }

        p.board = board;
        p.original = original;
        p.currentPuzzleLevel = levelIndex;
        p.boardSize = level.n;

        savePlayer(p);

        if (countedThisGame && levelIndex == 0 && p.beginnerWins >= 10)
            cout << "Beginner completed! Middle level unlocked.\n";

        if (countedThisGame && levelIndex == 1 && p.middleWins >= 50)
            cout << "Middle completed! Professional level unlocked.\n";
    }
}

Player signUp() {
    Player p;

    string name;

    cout << "\n========== SIGN UP ==========\n";
    cout << "Enter a one-word name (English letters/numbers): ";

    cin >> name;

    p.name = cleanName(name);

    p.id = generateID();

    savePlayer(p);

    cout << "\nAccount created!\n";
    cout << "Name: " << p.name << '\n';
    cout << "Your new Player ID: " << p.id << '\n';
    cout << "Please save this ID for your next sign-in.\n";

    return p;
}

bool signIn(Player& p) {
    string name, id;

    cout << "\n========== SIGN IN ==========\n";
    cout << "Enter your name: ";
    cin >> name;

    cout << "Enter your Player ID: ";
    cin >> id;

    if (!loadPlayer(name, id, p)) {
        cout << "Account not found. Check your name and ID, or sign up.\n";
        return false;
    }

    cout << "Welcome back, " << p.name
         << "! Your saved progress is loaded.\n";

    return true;
}

int main() {
    cout << "========================================\n";
    cout << "      SUDOKU QUEST - C++ TERMINAL\n";
    cout << "      File Handling (No filesystem)\n";
    cout << "========================================\n";

    while (true) {
        cout << "\n1. Sign Up (new player)\n";
        cout << "2. Sign In (existing player)\n";
        cout << "0. Exit\n";

        int choice = readInt("Choose: ", 0, 2);

        if (choice == 0) {
            cout << "Goodbye!\n";
            break;
        }

        Player player;

        if (choice == 1) {
            player = signUp();
        } else {
            if (!signIn(player))
                continue;
        }

        while (true) {
            showProgress(player);

            cout << "\n1. Beginner (4x4; 10 wins to unlock Middle)\n";
            cout << "2. Middle (6x6; 50 wins to unlock Professional)\n";
            cout << "3. Professional (9x9)\n";
            cout << "4. Resume saved puzzle\n";
            cout << "5. Sign out\n";
            cout << "0. Save and exit\n";

            int option = readInt("Select: ", 0, 5);

            if (option == 0) {
                savePlayer(player);
                cout << "Progress saved. Goodbye!\n";
                return 0;
            }

            if (option == 5) {
                savePlayer(player);
                break;
            }

            if (option == 4) {
                if (player.board.empty()) {
                    cout << "No saved puzzle yet. Choose a level first.\n";
                } else {
                    playGame(player, player.currentPuzzleLevel);
                }

                continue;
            }

            int levelIndex = option - 1;

            if (!levelUnlocked(player, levelIndex)) {
                cout << "This level is locked. Complete the required games first.\n";
                continue;
            }

            playGame(player, levelIndex);
        }
    }

    return 0;
}
