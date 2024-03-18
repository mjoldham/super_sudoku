#include "Grid.h"
#include <conio.h>
#include <iostream>

using namespace std;

int main()
{
	Sudoku::Grid grid;
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

		grid.Generate();
		grid.Draw(false);

		cout << "Press any key to show solution." << endl << endl;
		_getch();
		grid.Draw(true);
	}
}
