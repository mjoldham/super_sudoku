#include "Grid.h"
#include "ConstraintMatrix.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace chrono;
using std::cout;

namespace Sudoku
{
	Grid::Grid() : values{} {}

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

	void Grid::Generate()
	{
		auto start = high_resolution_clock::now();
		
		// Starts by shuffling the rows and finding the first solution to be used as our complete grid.
		ConstraintMatrix matrix = ConstraintMatrix();
		matrix.Generate(this);

		vector<int> pos;
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				pos.emplace_back(i * 9 + j);
			}
		}

		int count = 81;
		while (pos.size() > 0)
		{
			// Picks a random cell to erase.
			int index = RandomRange(pos.size());
			int p = pos[index];
			pos.erase(pos.begin() + index);

			int j = p % 9;
			int i = (p - j) / 9;
			ToggleHide(i, j);

			// If there's not a unique solution then restore the cell.
			if (!matrix.Solve(this))
			{
				ToggleHide(i, j);
			}
			else
			{
				count--;
			}
		}

		auto end = high_resolution_clock::now();
		cout << "Total time: " << duration_cast<milliseconds>(end - start).count() << "ms" << endl;
		cout << "Number of helpers: " << count << endl;
		cout << endl;
	}

	void Grid::Draw(bool showComplete) const
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
						int j = j1 * 3 + j2;
						if (showComplete)
						{
							cout << abs(values[i][j].trueValue) << " ";
						}
						else
						{
							cout << GetValue(i, j) << " ";
						}
					}
					cout << " ";
				}
				cout << "\n";
			}
			cout << "\n";
		}
	}

	void Grid::Draw(int highlightValue) const
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
						int j = j1 * 3 + j2;
						cout << GetHighlight(i, j, highlightValue) << " ";
					}
					cout << " ";
				}
				cout << "\n";
			}
			cout << "\n";
		}
	}

	int Grid::GetTrueValue(int i, int j) const
	{
		return values[i][j].trueValue;
	}

	char Grid::GetValue(int i, int j) const
	{
		int value = values[i][j].trueValue;
		if (value < 0)
		{
			value = values[i][j].playerValue;
		}

		if (!value)
		{
			return '_';
		}

		return '0' + value;
	}

	char Grid::GetHighlight(int i, int j, int value) const
	{
		// First checks for helper.
		if (values[i][j].trueValue > 0)
		{
			if (values[i][j].trueValue == value)
			{
				return '0' + value;
			}

			return 'x';
		}

		// If no helper checks for player value.
		if (values[i][j].playerValue)
		{
			if (values[i][j].playerValue == value)
			{
				return '0' + value;
			}

			return 'x';
		}

		// If no player value checks for pencil mark.
		if (values[i][j].pencilMarks & (1 << (value - 1)))
		{
			return 'o';
		}

		return '_';
	}

	void Grid::SetTrueValue(int i, int j, int value)
	{
		values[i][j].trueValue = value;
	}
	
	void Grid::SetPlayerValue(int i, int j, int value)
	{
		if (value < 0)
		{
			values[i][j].pencilMarks ^= 1 << (-value - 1);
			return;
		}

		if (values[i][j].playerValue == value)
		{
			values[i][j].playerValue = 0;
			return;
		}
		
		values[i][j].playerValue = value;
	}

	void Grid::Clear()
	{
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				values[i][j].playerValue = 0;
				values[i][j].pencilMarks = 0;
			}
		}
	}

	void Grid::ToggleHide(int i, int j)
	{
		values[i][j].trueValue = -values[i][j].trueValue;
	}

}
