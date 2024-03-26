#include "Grid.h"
#include "ConstraintMatrix.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <tuple>
#include <utility>
#include <variant>
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

	void Grid::Generate(bool usePositivePencilMarks)
	{
		auto start = high_resolution_clock::now();
		positivePencilmarks = usePositivePencilMarks;

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
				if (!positivePencilmarks)
				{
					BlockDigitFromPeers(values[i][j].trueValue, i, j);
				}
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

	void Grid::Draw(function<char(const CellValues(&)[9][9], int, int)> charFunc) const
	{
		cout << "    ";
		for (int j1 = 0; j1 < 3; j1++)
		{
			for (int j2 = 0; j2 < 3; j2++)
			{
				int j = j1 * 3 + j2;
				cout << j + 1 << " ";
			}
			cout << " ";
		}
		cout << "\n\n";

		for (int i1 = 0; i1 < 3; i1++)
		{
			for (int i2 = 0; i2 < 3; i2++)
			{
				int i = i1 * 3 + i2;
				cout << i + 1 << "   ";
				for (int j1 = 0; j1 < 3; j1++)
				{
					for (int j2 = 0; j2 < 3; j2++)
					{
						int j = j1 * 3 + j2;
						cout << charFunc(values, i, j) << " ";
					}
					cout << " ";
				}
				cout << "\n";
			}
			cout << "\n";
		}
	}

	void Grid::BlockDigitFromPeers(int value, int i, int j)
	{
		// Blocks row.
		for (int j1 = 0; j1 < 9; j1++)
		{
			if (j1 == j)
			{
				continue;
			}

			values[i][j1].pencilMarks |= 1 << (value - 1);
		}

		// Blocks column.
		for (int i1 = 0; i1 < 9; i1++)
		{
			if (i1 == i)
			{
				continue;
			}

			values[i1][j].pencilMarks |= 1 << (value - 1);
		}

		// Blocks box.
		int rowStart = (i / 3) * 3;
		int colStart = (j / 3) * 3;
		for (int i1 = rowStart; i1 < rowStart + 3; i1++)
		{
			for (int j1 = colStart; j1 < colStart + 3; j1++)
			{
				if (i1 == i && j1 == j)
				{
					continue;
				}

				values[i1][j1].pencilMarks |= 1 << (value - 1);
			}
		}
	}

	void Grid::Draw(bool showComplete) const
	{
		if (showComplete)
		{
			function<char(const CellValues(&)[9][9], int, int)> completeFunc =
				[](const CellValues(&cells)[9][9], int i, int j)
				{
					return (char)('0' + abs(cells[i][j].trueValue));
				};

			Draw(completeFunc);
		}
		else
		{
			function<char(const CellValues(&)[9][9], int, int)> playerFunc =
				[](const CellValues(&cells)[9][9], int i, int j)
				{
					int value = cells[i][j].trueValue;
					if (value < 0)
					{
						value = cells[i][j].playerValue;
					}

					if (!value)
					{
						return '_';
					}

					return (char)('0' + value);
				};

			Draw(playerFunc);
		}
	}

	void Grid::Draw(int highlightValue) const
	{
		auto highlightFunc =
			[](const CellValues(&cells)[9][9], int i, int j, int value, bool positive)
			{
				// First checks for helper.
				if (cells[i][j].trueValue > 0)
				{
					if (cells[i][j].trueValue == value)
					{
						return (char)('0' + value);
					}

					return 'x';
				}

				// If no helper checks for player value.
				if (cells[i][j].playerValue)
				{
					if (cells[i][j].playerValue == value)
					{
						return (char)('0' + value);
					}

					return 'x';
				}

				// If no player value checks for pencil mark.
				if (cells[i][j].pencilMarks & (1 << (value - 1)))
				{
					if (positive)
					{
						return 'o';
					}
					
					return 'x';
				}

				return '_';
			};
		using namespace placeholders;
		auto func = bind(highlightFunc, _1, _2, _3, highlightValue, positivePencilmarks);

		Draw(func);
	}

	void Grid::HiddenSubset(const vector<tuple<int, int, int>>& digitsCells)
	{
		int digitMask = 0b111111111;
		for (const auto& digitCell : digitsCells)
		{
			digitMask ^= 1 << (get<0>(digitCell) - 1);
		}

		for (const auto& digitCell : digitsCells)
		{
			values[get<1>(digitCell)][get<2>(digitCell)].pencilMarks |= digitMask;
		}
	}

	void Grid::NakedSubset(const vector<tuple<int, int, int>>& digitsCells)
	{
		int digitMask = 0, rowMask = 0, colMask = 0, boxMask = 0;
		for (const auto& digitCell : digitsCells)
		{
			digitMask |= 1 << (get<0>(digitCell) - 1);

			int i = get<1>(digitCell);
			int j = get<2>(digitCell);
			int ij = (i / 3) * 3 + j / 3;

			rowMask |= 1 << i;
			colMask |= 1 << j;
			boxMask |= 1 << ij;
		}

		// If the cells lie on a single row, eliminates the chosen digits from the row peers.
		int row = get<1>(digitsCells[0]);
		if (!(rowMask & (rowMask - 1)))
		{
			for (int j = 0; j < 9; j++)
			{
				if (1 << j & colMask)
				{
					continue;
				}

				values[row][j].pencilMarks |= digitMask;
			}
		}

		// If the cells lie on a single column, eliminates the chosen digits from the column peers.
		int col = get<2>(digitsCells[0]);
		if (!(colMask & (colMask - 1)))
		{
			for (int i = 0; i < 9; i++)
			{
				if (1 << i & rowMask)
				{
					continue;
				}

				values[i][col].pencilMarks |= digitMask;
			}
		}

		if (!(boxMask & (boxMask - 1)))
		{
			int rowStart = (row / 3) * 3;
			int colStart = (col / 3) * 3;
			for (int i = rowStart; i < rowStart + 3; i++)
			{
				for (int j = colStart; j < colStart + 3; j++)
				{
					if ((1 << i & rowMask) && (1 << j & colMask))
					{
						continue;
					}

					values[i][j].pencilMarks |= digitMask;
				}
			}
		}
	}

	void Grid::PointingSubset(int digit, int box, int rowcol)
	{

	}

	void Grid::ClaimingSubset(int digit, int box, int rowcol)
	{

	}

	int Grid::GetTrueValue(int i, int j) const
	{
		return values[i][j].trueValue;
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
			if (positivePencilmarks)
			{
				return 'o';
			}

			return 'x';
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
			values[i][j].playerValue = 0; // TODO: check houses of each peer and undo blocks if not positive! Could try running flags for all 27 houses.
			return;
		}
		
		values[i][j].playerValue = value;

		if (!positivePencilmarks)
		{
			BlockDigitFromPeers(value, i, j);
		}
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
