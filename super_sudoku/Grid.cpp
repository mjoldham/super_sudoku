#include "Grid.h"
#include "ConstraintMatrix.h"

#include <iostream>

using namespace std;

namespace Sudoku
{
	Grid::Grid() : values{} {}

	void Grid::Generate()
	{
		// Shuffle rows then get first solution as our complete grid.
		// Remove a random number and solve: if no solution or multiple, pick different number; else repeat.
		ConstraintMatrix matrix = ConstraintMatrix();
		int iter = 0;
		matrix.Search(this, ConstraintMatrix::SearchOption::FindFirst, iter);
	}

	void Grid::Draw()
	{
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
						int value = values[i][j1 * 3 + j2];
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

	void Grid::Set(int i, int j, int value)
	{
		values[i][j] = value;
	}
}
