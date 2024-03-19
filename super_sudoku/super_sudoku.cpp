#include "Grid.h"

#include <algorithm>
#include <conio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

void ParseInput(const string& str, int& i, int& j, int& value)
{
	i = clamp(str[0] - 1 - '0', 0, 8);
	j = clamp(str[1] - 1 - '0', 0, 8);
	value = clamp(str[2] - '0', 1, 9);
}

int main()
{
	Sudoku::Grid grid;
	while (true)
	{
		cout << "Generate new grid? [y/n]" << endl;
		string input;
		cin >> input;
		cout << endl;

		if (input.compare("y"))
		{
			break;
		}

		grid.Generate();
		grid.Draw(false);

		cout << "Enter row (1-9), column (1-9), and value (1-9) to fill cells." << endl;
		cout << "[S]how solution." << endl;

		while (true)
		{
			cin >> input;
			if (!input.compare("s"))
			{
				break;
			}

			int i, j, value;
			ParseInput(input, i, j, value);

			if (grid.GetTrueValue(i, j) > 0)
			{
				continue;
			}

			grid.SetPlayerValue(i, j, value);
			grid.Clear();
			grid.Draw(false);
		}

		grid.Draw(true);
	}
}
