#pragma once

#include <array>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>

namespace Sudoku
{
	class Grid
	{
	private:
		// When true, pencil marks indicate what a cell can contain, else they indicate what it cannot contain.
		bool positivePencilmarks = true;
		struct CellValues
		{
			int trueValue : 5, playerValue : 5, pencilMarks : 9;
		} values[9][9];

		void Draw(std::function<char(const CellValues(&)[9][9], int, int)>) const;
		void BlockDigitFromPeers(int, int, int);

	public:
		Grid();

		void Generate(bool);

		void Draw(bool) const;
		void Draw(int) const;

		// Occurs when N digits only have candidates in N cells in a house.
		// Eliminates all other candidates from those cells.
		void HiddenSubset(const std::vector<std::tuple<int, int, int>>&);

		// Occurs when N cells in a house only have candidates for N digits.
		// Eliminates those candidates from the house.
		// If they're in an intersection then they're a locked subset that eliminates from both houses.
		void NakedSubset(const std::vector<std::tuple<int, int, int>>&);

		// Occurs when all candidates for a digit in a box are in an intersection with a row/column.
		// Eliminates those candidates from the row/column outside the box.
		void PointingSubset(int, int, int); // value, box (0-8), row/col (0-5)

		// Occurs when all candidates for a digit in a row/column are in an intersection with a box.
		// Eliminates those candidates from the box outside the row/column.
		void ClaimingSubset(int, int, int); // value, box (0-8), row/col (0-5)

		int GetTrueValue(int, int) const;
		char GetHighlight(int, int, int) const;

		void SetTrueValue(int, int, int);
		void SetPlayerValue(int, int, int);

		void Clear();

		void ToggleHide(int, int);
	};
}