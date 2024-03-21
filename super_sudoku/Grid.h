#pragma once

namespace Sudoku
{
	class Grid
	{
	private:
		struct
		{
			int trueValue : 5, playerValue : 5, pencilMarks : 9;
		} values[9][9];

	public:
		Grid();

		void Generate();
		void Draw(bool) const;
		void Draw(int) const;

		int GetTrueValue(int, int) const;
		char GetValue(int, int) const;
		char GetHighlight(int, int, int) const;

		void SetTrueValue(int, int, int);
		void SetPlayerValue(int, int, int);

		void Clear();

		void ToggleHide(int, int);
	};
}