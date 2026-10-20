#include "httplib.h"
#include "json.hpp"
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#ifdef _WIN32
  #include <windows.h>
#endif

using json = nlohmann::json;
using namespace std;

bool isSafe(const vector<vector<int>>& b, int r, int c, int value, int br, int bc) {
    int n = (int)b.size();
    for (int i = 0; i < n; ++i) {
        if (b[r][i] == value || b[i][c] == value) return false;
    }
    int startR = (r / br) * br, startC = (c / bc) * bc;
    for (int i = startR; i < startR + br; ++i)
        for (int j = startC; j < startC + bc; ++j)
            if (b[i][j] == value) return false;
    return true;
}

bool validateShape(const json& input, vector<vector<int>>& board, int& br, int& bc) {
    if (!input.contains("board") || !input["board"].is_array()) return false;
    int n = input.value("size", 0);
    br = input.value("boxRows", 0); bc = input.value("boxCols", 0);
    if (!(n == 4 || n == 6 || n == 9) || br * bc != n || n % br != 0 || n % bc != 0) return false;
    if ((int)input["board"].size() != n) return false;
    board.assign(n, vector<int>(n, 0));
    for (int r = 0; r < n; ++r) {
        if (!input["board"][r].is_array() || (int)input["board"][r].size() != n) return false;
        for (int c = 0; c < n; ++c) {
            if (!input["board"][r][c].is_number_integer()) return false;
            int v = input["board"][r][c].get<int>();
            if (v < 0 || v > n) return false;
            board[r][c] = v;
        }
    }
    return true;
}

bool boardHasConflicts(const vector<vector<int>>& b, int br, int bc, vector<int>& conflicts) {
    int n = (int)b.size(); bool bad = false;
    auto add = [&](int r, int c) { conflicts.push_back(r*n+c); bad = true; };
    for (int r = 0; r < n; ++r) for (int c = 0; c < n; ++c) {
        int v = b[r][c]; if (!v) continue;
        for (int k = c + 1; k < n; ++k) if (b[r][k] == v) { add(r,c); add(r,k); }
        for (int k = r + 1; k < n; ++k) if (b[k][c] == v) { add(r,c); add(k,c); }
        int sr=(r/br)*br, sc=(c/bc)*bc;
        for(int rr=sr;rr<sr+br;++rr) for(int cc=sc;cc<sc+bc;++cc)
            if ((rr>r || (rr==r && cc>c)) && b[rr][cc]==v) { add(r,c); add(rr,cc); }
    }
    sort(conflicts.begin(), conflicts.end()); conflicts.erase(unique(conflicts.begin(), conflicts.end()), conflicts.end());
    return bad;
}

bool solveSudoku(vector<vector<int>>& b, int br, int bc) {
    int n=(int)b.size();
    for(int r=0;r<n;++r) for(int c=0;c<n;++c) if(b[r][c]==0) {
        for(int value=1;value<=n;++value) if(isSafe(b,r,c,value,br,bc)) {
            b[r][c]=value;
            if(solveSudoku(b,br,bc)) return true;
            b[r][c]=0; // Backtrack: undo a choice that did not lead to a solution.
        }
        return false;
    }
    return true;
}

json boardToJson(const vector<vector<int>>& board) { return json{{"board", board}}; }

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    httplib::Server server;
    server.set_mount_point("/", "./frontend");

    server.Get("/api/status", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"ok":true,"message":"Sudoku C++ backend is running"})", "application/json");
    });

    server.Post("/api/solve", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json input = json::parse(req.body);
            vector<vector<int>> board; int br=0,bc=0;
            if(!validateShape(input,board,br,bc)) {
                res.status=400; res.set_content(R"({"ok":false,"message":"Invalid board format."})","application/json"); return;
            }
            vector<int> conflicts;
            if(boardHasConflicts(board,br,bc,conflicts)) {
                res.set_content(json{{"ok",false},{"message","The board contains duplicate numbers."},{"conflicts",conflicts}}.dump(),"application/json"); return;
            }
            if(!solveSudoku(board,br,bc)) {
                res.set_content(R"({"ok":false,"message":"No solution exists for this puzzle."})","application/json"); return;
            }
            json output=boardToJson(board); output["ok"]=true;
            res.set_content(output.dump(),"application/json");
        } catch(...) {
            res.status=400; res.set_content(R"({"ok":false,"message":"Could not read request JSON."})","application/json");
        }
    });

    server.Post("/api/check", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json input=json::parse(req.body);
            vector<vector<int>> board; int br=0,bc=0;
            if(!validateShape(input,board,br,bc)) {
                res.status=400; res.set_content(R"({"valid":false,"message":"Invalid board format."})","application/json"); return;
            }
            vector<int> conflicts;
            if(boardHasConflicts(board,br,bc,conflicts)) {
                res.set_content(json{{"valid",false},{"conflicts",conflicts}}.dump(),"application/json"); return;
            }
            bool full=true; for(const auto& row:board) for(int v:row) if(v==0) full=false;
            res.set_content(json{{"valid",full},{"conflicts",json::array()}}.dump(),"application/json");
        } catch(...) {
            res.status=400; res.set_content(R"({"valid":false,"message":"Could not read request JSON."})","application/json");
        }
    });

    cout << "Sudoku Quest server is running at http://localhost:8080\n";
    cout << "Keep this terminal open while playing. Press Ctrl+C to stop.\n";
    if(!server.listen("127.0.0.1",8080)) {
        cerr << "Could not start server. Port 8080 may already be in use.\n";
        return 1;
    }
    return 0;
}

