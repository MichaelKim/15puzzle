#include <iostream>
#include <vector>

#include "../include/Direction.h"
#include "../include/DisjointDatabase.h"

using namespace std;

int main() {
    DisjointDatabase db({{
         {1, 2, 3, 0},
         {5, 6, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}
    },{
         {0, 0, 0, 4},
         {0, 0, 7, 8},
         {0, 0, 11, 12},
         {0, 0, 0, 0}
    },{
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {9, 10, 0, 0},
         {13, 14, 15, 0}
    }});

    db.setup();

    vector<Direction> solution = db.solve({
        {1, 2, 3, 4},
        {5, 6, 8, 12},
        {9, 10, 7, 0},
        {13, 14, 11, 15}
        // { 1,  9, 11,  4},
        // {14,  8,  2,  7},
        // {10,  6,  3, 12},
        // { 5,  0, 13, 15}
    });

    cout << "Solution:" << endl;
    for(Direction dir: solution) {
        cout << (int) dir << " ";
    }
    cout << endl;
    return 0;
}
