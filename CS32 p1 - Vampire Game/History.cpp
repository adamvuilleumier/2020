#include "History.h"
#include "globals.h"
#include <cctype>
#include <iostream>

using namespace std;

History::History(int nRows, int nCols)
{
	rows = nRows;
	cols = nCols;
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			grid[r][c] = '.';
}

bool History::record(int r, int c)
{
	if (r > rows || c > cols)
		return false;

	else if (grid[r-1][c-1] == '.')
		grid[r-1][c-1] = 'A';
	else if (isalpha(grid[r-1][c-1]) && grid[r-1][c-1] != 'Z')
		grid[r-1][c-1]++;
	else if (grid[r-1][c-1] == 'Z')
		return true;

	return true;
}

void History::display() const
{
	clearScreen();

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++)
			cout << grid[r][c];
		cout << '\n';
	}

	cout << '\n';
}