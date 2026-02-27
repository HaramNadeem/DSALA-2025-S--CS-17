// 2025(S)-CS-17

#include <iostream>
using namespace std;

int main() {
    int n = 8; 
    int w = n + 1;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= w; j++) {
            if (j == 1 || j == w)
                {cout << "*";}
            else if (i == 2 && (j == 2 || j == w - 1))
                {cout << "*";}
            else if (i == n - 1 && (j == 2 || j == w - 1))
                {cout << "*";}
            else if (i == n / 2 + 1 && (j == n / 2 + 1 || j == w - n / 2))
                {cout << "*";}
            else
                {cout << " ";}
        }
        cout << endl;
    }

}