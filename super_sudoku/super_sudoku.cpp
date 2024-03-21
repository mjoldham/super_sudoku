#include "Grid.h"

#include <algorithm>
#include <cmath>
#include <conio.h>
#include <functional>
#include <iostream>
#include <stdlib.h>

using namespace std;

inline int ToInt(const string& str, int index)
{
	return str[index] - '0';
}

enum InputType
{
	Invalid = 0,
	Highlight = 1,
	Set = 2
};

InputType ParseInput(const string& str, int& i, int& j, int& value)
{
	function<bool(int)> valueIsValid = [](int v) { return v > 0 && v < 10; };
	int allValid = 0;
	int valSign = 1;
	switch (str.size())
	{
	case 1:
		value = ToInt(str, 0);
		return (InputType)valueIsValid(value);

	case 4:
		if (str[3] == 'p')
		{
			valSign = -1;
		}
		[[fallthrough]];

	case 3:
		i = ToInt(str, 0) - 1;
		j = ToInt(str, 1) - 1;
		value = valSign * ToInt(str, 2);

		allValid = i > -1 && i < 9 && j > -1 && j < 9 && valueIsValid(valSign * value);
		return (InputType)(allValid * (int)InputType::Set);

	default:
		return InputType::Invalid;
	}
}

void PrintInstructions()
{
	cout << "Enter row (1-9), column (1-9), and value (1-9) to fill cells.\n";
	cout << "Append 'p' to enter/erase a pencil mark (e.g. 145p = pencil mark 5 in (1,4)).\n\n";
	cout << "Enter single value (1-9) to highlight all cells with that value / pencil mark.\n";
	cout << "Repeat this value to return to the normal view.\n\n";
	cout << "Show [s]olution.\n" << endl;
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

		PrintInstructions();

		grid.Clear();
		grid.Generate();
		grid.Draw(false);

		int highlight = 0, value = 0;
		InputType inputType;
		while (true)
		{
			cin >> input;
			if (!input.compare("s"))
			{
				break;
			}

			int i, j;
			inputType = ParseInput(input, i, j, value);
			if (inputType == InputType::Invalid)
			{
				cout << "Entered input is invalid. Try again." << endl;
				continue;
			}
			
			if (inputType == InputType::Highlight)
			{
				if (highlight == value)
				{
					highlight = 0;
					grid.Draw(false);
				}
				else
				{
					highlight = value;
					grid.Draw(value);
				}

				continue;
			}

			grid.SetPlayerValue(i, j, value);

			if (value < 0)
			{
				highlight = -value;
				grid.Draw(-value);
			}
			else if (value == highlight)
			{
				grid.Draw(value);
			}
			else
			{
				highlight = 0;
				grid.Draw(false);
			}
		}

		grid.Draw(true);
	}
}
