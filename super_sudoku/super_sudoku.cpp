// super_sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>

using namespace std;
using namespace chrono;

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
	srand(time(NULL));
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

inline int GetBoxIndex(int i, int j)
{
	return (i / 3) * 3 + j / 3; // Some integer truncation trickery!
}

inline void GetGridIndices(int inPos, int& outI, int& outJ)
{
	outJ = inPos % 9;
	outI = (inPos - outJ) / 9;
}

inline int GetRowConstraint(int i, int cand = 0)
{
	return 81 + i * 9 + cand;
}

inline int GetColumnConstraint(int j, int cand = 0)
{
	return 81 * 2 + j * 9 + cand;
}

inline int GetBoxConstraint(int i, int j, int cand = 0)
{
	return 81 * 3 + GetBoxIndex(i, j) * 9 + cand;
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
			tally.subCount[GetBoxIndex(i, j)][value - 1]++;
		}
	}

	return tally;
}

void DrawGrid(const Grid9x9& grid)
{
	/*for (int i = 0; i < 23; i++)
	{
		cout << " ";
	}

	for (int i = 0; i < 9; i++)
	{
		cout << " " << i + 1 << ":";
	}
	cout << endl;
	
	NumberTally tally = CountRepeats(grid);*/
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

			/*cout << "\b";
			for (int k = 0; k < 9; k++)
			{
				cout << "  " << tally.rowCount[i][k];
			}*/
			cout << endl;
		}
		cout << endl;
	}

	/*for (int i = 0; i < 23; i++)
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
	}*/
}

class ConstraintMatrix
{
	// There are 9 candidates for each of the 81 cells, represented by the rows.
	// (e.g. a 5 at (2,3) is represented by its own row.)
	
	// There are 4 constraints represented by columns that each row satisfies:
	// 1) One number for each of the 81 cells.
	// 2) 9 unique numbers in each of the 9 rows.
	// 3) 9 unique numbers in each of the 9 columns.
	// 4) 9 unique numbers in each of the 9 boxes.
	
	// (e.g. 5 at (2,3) satisfies having 1) a number in cell (2,3), 2) a 5 in the 2nd row,
	// 3) a 5 in the 3rd column, and 4) a 5 in the 1st box.)
	
private:
	static const int rowCount = 9 * 81, colCount = 4 * 81;
	bool values[rowCount][colCount];

	void DeleteRow(int r, int i, int j, int cand)
	{
		int row = GetRowConstraint(i, cand);
		int col = GetColumnConstraint(j, cand);
		int box = GetBoxConstraint(i, j, cand);
		values[r][i * 9 + j] = values[r][row] = values[r][col] = values[r][box] = 0;
	}

public:
	ConstraintMatrix() : values{}
	{
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				int pos = i * 9 + j;
				int row = GetRowConstraint(i);
				int col = GetColumnConstraint(j);
				int box = GetBoxConstraint(i, j);;
				for (int cand = 0; cand < 9; cand++)
				{
					// Satisfies cell, row, column, and box constraints.
					int r = pos * 9 + cand;
					values[r][pos] = values[r][row + cand] = values[r][col + cand] = values[r][box + cand] = 1;
				}
			}
		}
	}

	void DeleteRowsColumns(int pos, int i1, int j1, int cand1)
	{
		// Deletes rows that satisfy the same constraints.
		int row = GetRowConstraint(i1, cand1);
		int col = GetColumnConstraint(j1, cand1);
		int box = GetBoxConstraint(i1, j1, cand1);

		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				for (int cand = 0; cand < 9; cand++)
				{
					int r = (i * 9 + j) * 9 + cand;
					if (values[r][pos] || values[r][row] || values[r][col] || values[r][box])
					{
						DeleteRow(r, i, j, cand);
					}
				}
			}
		}
	}

	void Print(int posLen, int constrLen)
	{
		for (int r1 = 0; r1 < posLen; r1++)
		{
			for (int r2 = 0; r2 < 9; r2++)
			{
				for (int c1 = 0; c1 < 4; c1++)
				{
					for (int c2 = 0; c2 < constrLen; c2++)
					{
						cout << " " << (int)values[r1 * 9 + r2][c1 * 81 + c2];
					}
					cout << " ";
				}
				cout << endl;
			}
			cout << endl;
		}
	}

	const int& RowCount() const
	{
		return rowCount;
	}

	const int& ColCount() const
	{
		return colCount;
	}

	const bool& operator()(int i, int j) const
	{
		return values[i][j];
	}
	
	bool& operator()(int i, int j)
	{
		return values[i][j];
	}
};

int iter = 0, totalIter = 0, incr = 0, incrSize = 1000;
void TickIterCounter()
{
	totalIter++;
	if (++iter / incrSize > incr)
	{
		if (incr > 0)
		{
			for (int i = 0; i < log10(iter); i++)
			{
				cout << "\b";
			}
		}
		incr = iter / incrSize;
		cout << iter;
	}
}

steady_clock::time_point startTime;

inline void StartTimer()
{
	startTime = high_resolution_clock::now();
}

inline long GetTimeSinceStart()
{
	return duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
}

bool XSolver(ConstraintMatrix& matrix, Grid9x9& grid, const array<int, 9 * 81>& rowIndexer, int timeOut = 300,
	int pos = 0, int r1 = 0)
{
	if (pos > 80) // If all cells filled, exact-cover problem is solved so terminate successfully.
	{
		return true;
	}

	if (GetTimeSinceStart() >= timeOut)
	{
		return false;
	}

	TickIterCounter();
	bool isSuccessful = false;
	
	r1 = max(r1, pos * 9);
	int endOfCands = pos * 9 + 9;
	for (; r1 < endOfCands; r1++)
	{
		// Chooses a random candidate for a cell.
		if (matrix(rowIndexer[r1], pos))
		{
			isSuccessful = true;
			break;
		}
	}

	if (!isSuccessful)
	{
		return false;
	}

	// Adds chosen candidate to the grid.
	int i, j;
	GetGridIndices(pos, i, j);
	int cand = rowIndexer[r1] % 9;
	grid.values[i][j] = cand + 1;

	ConstraintMatrix* mat1 = new ConstraintMatrix(matrix);
	mat1->DeleteRowsColumns(pos, i, j, cand);

	if (XSolver(*mat1, grid, rowIndexer, timeOut, pos + 1))
	{
		delete mat1;
		return true;
	}
	delete mat1;

	// If the solver fails, then we must choose a different candidate for this position.
	for (r1++; r1 < endOfCands; r1++)
	{
		if (XSolver(matrix, grid, rowIndexer, timeOut, pos, r1))
		{
			return true;
		}
	}

	// If none of the candidates for this cell succeed, pass back to the previous position.
	return false;
}

int main()
{
	Grid9x9 grid;
	array<int, 9 * 81> rowIndexer{ };
	for (int r = 0; r < rowIndexer.size(); r++)
	{
		rowIndexer[r] = r;
	}

	while (true)
	{
		cout << "Generate new grid? [y/n]" << endl;
		string response;
		cin >> response;
		cout << endl;

		if (response.compare("y"))
		{
			break;
		}

		ConstraintMatrix matrix;

		int shuffles = totalIter = 0;
		auto start = high_resolution_clock::now();
		do
		{
			iter = incr = 0;
			cout << endl << "Shuffle " << ++shuffles << ": ";

			random_device rd;
			mt19937 g(rd());

			for (int pos = 0; pos < 81; pos++)
			{
				shuffle(rowIndexer.begin() + pos * 9, rowIndexer.begin() + pos * 9 + 9, g);
			}

			StartTimer();
		}
		while (!XSolver(matrix, grid, rowIndexer));
		auto end = high_resolution_clock::now();

		cout << endl;
		cout << "Total iterations: " << totalIter << endl;
		cout << "Total time: " << duration_cast<milliseconds>(end - start).count() << "ms" << endl;
		cout << endl;

		DrawGrid(grid);
	}
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
