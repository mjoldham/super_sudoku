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
		void Draw();
		void Set(int, int, int);
	};
}