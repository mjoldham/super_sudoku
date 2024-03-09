// super_sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
#include <algorithm>
#include <cstdlib>
using namespace std;

int RandomRange(int n)
{
	n = clamp(n, 0, RAND_MAX);
	const int bucketSize = RAND_MAX / n;

	int r;
	do
	{
		r = rand() / bucketSize;
	}
	while (r >= n);

	return r;
}

struct Grid9x9
{
	int values[9][9];
};

Grid9x9 GenerateGrid()
{
	srand(time(0));
	Grid9x9 grid;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			grid.values[i][j] = RandomRange(10);
		}
	}

	return grid;
}

void DrawGrid(const Grid9x9& grid)
{
	for (int j1 = 0; j1 < 3; j1++)
	{
		for (int j2 = 0; j2 < 3; j2++)
		{
			int j = j1 * 3 + j2;
			cout << "  ";
			for (int i1 = 0; i1 < 3; i1++)
			{
				for (int i2 = 0; i2 < 3; i2++)
				{
					int value = grid.values[i1 * 3 + i2][j];
					if (value == 0)
					{
						cout << "_ ";
					}
					else
					{
						cout << value << " ";
					}
				}
				cout << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
}

int main()
{
	Grid9x9 grid = GenerateGrid();
	DrawGrid(grid);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
