#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>

using namespace std;

class State {
public:
    vector<vector<int>> board;
    int blankx;
    int blanky;
    int dist;

    State() {}

    State(const State& s) {
        this->board = s.board;
        this->blankx = s.blankx;
        this->blanky = s.blanky;
        this->dist = s.dist;
    }

    State(vector<vector<int>> newBoard, int newDist = 0) {
        this->dist = newDist;
        this->board = newBoard;
        for(int i = 0; i < newBoard.size(); i++) {
            for(int j = 0; j < newBoard[i].size(); j++) {
                if (newBoard[i][j] == -1) {
                    this->blankx = j;
                    this->blanky = i;
                    return;
                }
            }
        }
    }

    string getID(bool blank) { // blank: true => ignore blanks
        string boardID = "";
        vector<int> arr;
        for(auto row: this->board) {
            for(int i: row) {
                if (blank && i == -1) boardID += "0 ";
                else boardID += to_string(i) + " ";
            }
        }
        return boardID;
    }

    vector<shared_ptr<State>> getChildren() {
        vector<shared_ptr<State>> c;
        if (blanky > 0) { // Shift up
            shared_ptr<State> up(new State(*this));

            if (up->board[this->blanky - 1][this->blankx] > 0) {
                up->dist++;
            }
            swap(up->board[this->blanky][this->blankx], up->board[this->blanky - 1][this->blankx]);
            up->blanky--;

            c.push_back(up);
        }
        if (blankx > 0) {
            shared_ptr<State> left(new State(*this));

            if (left->board[this->blanky][this->blankx - 1] > 0) {
                left->dist++;
            }
            swap(left->board[this->blanky][this->blankx], left->board[this->blanky][this->blankx - 1]);
            left->blankx--;

            c.push_back(left);
        }
        if (blanky < this->board.size() - 1) {
            shared_ptr<State> down(new State(*this));

            if (down->board[this->blanky][this->blankx - 1] > 0) {
                down->dist++;
            }
            swap(down->board[this->blanky][this->blankx], down->board[this->blanky + 1][this->blankx]);
            down->blanky++;

            c.push_back(down);
        }
        if (blankx < this->board[0].size() - 1) {
            shared_ptr<State> right(new State(*this));

            if (right->board[this->blanky][this->blankx - 1] > 0) {
                right->dist++;
            }
            swap(right->board[this->blanky][this->blankx], right->board[this->blanky][this->blankx + 1]);
            right->blankx++;

            c.push_back(right);
        }
        return c;
    }
};

void generateDatabase(vector<vector<int>> startBoard, string filename) {
    unordered_set<string> checked;
    unordered_map<string, int> database;

    shared_ptr<State> state (new State(startBoard));

    queue<shared_ptr<State>> bfs;
    bfs.push(state);

    while(!bfs.empty()) {
        shared_ptr<State> curr = bfs.front();
        bfs.pop();

        string stateID = curr->getID(false);
        auto it = checked.find(stateID);
        if (it == checked.end()) {
            checked.insert(stateID);

            vector<shared_ptr<State>> children = curr->getChildren();
            for(auto c: children) {
                bfs.push(c);
            }
        }

        string stateID2 = curr->getID(true);
        auto it2 = database.find(stateID2);
        if (it2 == database.end() || database[stateID2] > curr->dist) {
            database[stateID2] = curr->dist;
            cout << stateID2 << " " << database.size() << endl;
        }
    }

    // Store file
    ofstream file("database-" + filename + ".txt");
    if (!file.good()) {
        cerr << "Could not generate database file: database-" + filename + ".txt" << endl;
        return;
    }

    for (auto it: database) {
        file << it.first << " " << it.second << endl;
    }

    file.close();
}

int main() {
    cout << "Generating a disjoint pattern database (6-6-3):" << endl;

/*    cout << "Generating 6 tile database (left):" << endl;
    generateDatabase({
             {1, 0, 0, 0},
             {5, 6, 0, 0},
             {9, 10, 0, 0},
             {13, 0, 0, -1}
    }, "6a");

    cout << "Generating 6 tile database (right):" << endl;
    generateDatabase({
             {0, 0, 0, 0},
             {0, 0, 7, 8},
             {0, 0, 11, 12},
             {0, 14, 15, -1}
    }, "6b");*/

    cout << "Generating 3 tile database:" << endl;
    generateDatabase({
             {0, 2, 3, 4},
             {0, 0, 0, 0},
             {0, 0, 0, 0},
             {0, 0, 0, -1}
    }, "3");
    return 0;
}
