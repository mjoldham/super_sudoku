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
		void Clear();

		int GetTrueValue(int, int) const;
		int GetPlayerValue(int, int) const;
		int GetPencilMarks(int, int) const;
		bool GetPencilMark(int, int, int) const;

		void SetTrueValue(int, int, int);
		void SetPlayerValue(int, int, int);
		void SetPencilMarks(int, int, int);

		void ToggleHide(int, int);
	};
}