#pragma once

namespace Sudoku
{
	class Grid
	{
	private:
		int values[9][9];
	public:
		Grid();

		void Generate();
		void Draw(bool);
		const int Get(int, int) const;
		void Set(int, int, int);
		void ToggleHide(int, int);
	};
}