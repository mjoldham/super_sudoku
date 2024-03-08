// super_sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::string;

void GenerateGrid()
{
	for (int j1 = 0; j1 < 3; j1++)
	{
		for (int j2 = 0; j2 < 3; j2++)
		{
			cout << "  ";
			for (int i1 = 0; i1 < 3; i1++)
			{
				for (int i2 = 0; i2 < 3; i2++)
				{
					cout << "_ ";
				}
				cout << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
}

int main()
{
	GenerateGrid();
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
