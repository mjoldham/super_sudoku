// super_sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <bitset>

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
	Grid9x9() : values{ 0 }
	{
	}
};

struct NumberTally
{
	int rowCount[9][9], colCount[9][9], subCount[9][9];
	NumberTally() : rowCount{ 0 }, colCount{ 0 }, subCount{ 0 }
	{}
};

Grid9x9 GenerateGrid()
{
	srand(time(0));
	Grid9x9 grid;
	NumberTally tally;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			int value = RandomRange(10);
			if (value != 0)
			{
				int valIndex = value - 1;
				int ij = (i / 3) * 3 + j / 3;
				if (tally.rowCount[i][valIndex] > 0 ||
					tally.colCount[j][valIndex] > 0 ||
					tally.subCount[ij][valIndex] > 0)
				{
					value = 0;
				}
				else
				{
					tally.rowCount[i][valIndex] = tally.colCount[j][valIndex] = tally.subCount[ij][valIndex] = 1;
				}
			}

			grid.values[i][j] = value;
		}
	}

	return grid;
}

NumberTally CountRepeats(Grid9x9 inGrid)
{
	// Checks the rows, columns, and 3x3 subgrids.
	NumberTally tally;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			int value = inGrid.values[i][j];
			if (value == 0)
			{
				continue;
			}

			tally.rowCount[i][value - 1]++;
			tally.colCount[j][value - 1]++;
			tally.subCount[(i / 3) * 3 + j / 3][value - 1]++; // Some integer truncation trickery!
		}
	}

	return tally;
}

void DrawGrid(const Grid9x9& grid)
{
	for (int i = 0; i < 23; i++)
	{
		cout << " ";
	}

	for (int i = 0; i < 9; i++)
	{
		cout << " " << i + 1 << ":";
	}
	cout << endl;
	
	NumberTally tally = CountRepeats(grid);
	for (int i1 = 0; i1 < 3; i1++)
	{
		for (int i2 = 0; i2 < 3; i2++)
		{
			int i = i1 * 3 + i2;
			cout << "  ";
			for (int j1 = 0; j1 < 3; j1++)
			{
				for (int j2 = 0; j2 < 3; j2++)
				{
					int value = grid.values[i][j1 * 3 + j2];
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

			cout << "\b";
			for (int k = 0; k < 9; k++)
			{
				cout << "  " << tally.rowCount[i][k];
			}
			cout << endl;
		}
		cout << endl;
	}

	for (int i = 0; i < 23; i++)
	{
		cout << " ";
	}

	for (int i = 0; i < 9; i++)
	{
		cout << " s" << i << ":";
	}
	cout << endl;

	for (int k = 0; k < 9; k++)
	{
		cout << k + 1 << ":";
		for (int j1 = 0; j1 < 3; j1++)
		{
			for (int j2 = 0; j2 < 3; j2++)
			{
				cout << tally.colCount[j1 * 3 + j2][k] << " ";
			}
			cout << " ";
		}

		cout << "\b";
		for (int ij = 0; ij < 9; ij++)
		{
			cout << "   " << tally.subCount[ij][k];
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
