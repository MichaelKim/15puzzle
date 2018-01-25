#include <iostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"

using namespace std;

int main() {
    DisjointDatabase db({{
         {1, 2, 3, 4},
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}
    },{
         {0, 0, 0, 0},
         {5, 6, 7, 8},
         {0, 0, 0, 0},
         {0, 0, 0, 0}
    },{
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {9, 10, 11, 12},
         {0, 0, 0, 0}
    },{
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {13, 14, 15, 0}
    }});

    db.setup();

    vector<Direction> solution = db.solve({
        {1, 2, 3, 8},
        {5, 6, 7, 4},
        {9, 10, 11, 12},
        {13, 14, 15, 0}
        /*{ 1,  9, 11,  4},
        {14,  8,  2,  7},
        {10,  6,  3, 12},
        { 5,  0, 13, 15}*/
    });

    cout << "Solution:" << endl;
    for(Direction dir: solution) {
        cout << (int) dir << " ";
    }
    cout << endl;
    return 0;
}
