//2025(S)-CS-17

#include <iostream>
using namespace std;

int main() {
    const int ROWS = 4, COLS = 5;
    int matrix[ROWS][COLS] = {0};

    int value = 1;
    int top = 0, bottom = ROWS - 1;
    int left = 0, right = COLS - 1;

    while (top <= bottom && left <= right) {
       
        for (int i = left; i <= right; ++i)
           {matrix[top][i] = value++;}
        ++top;

     
        for (int i = top; i <= bottom; ++i)
            {matrix[i][right] = value++;}
        --right;


        if (top <= bottom) {
            for (int i = right; i >= left; --i)
                {matrix[bottom][i] = value++;}
            --bottom;
        }

        if (left <= right) {
            for (int i = bottom; i >= top; --i)
                {matrix[i][left] = value++;}
            ++left;
        }
    }

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j)
            {cout << matrix[i][j] << " ";}
        cout << endl;
    }

}


