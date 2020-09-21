#include "Arena.h"
#include "History.h"
#include "globals.h"
#include <iostream>
using namespace std;

int main()
{
	Arena a(1, 4);
	a.addPlayer(1, 4);
	a.addVampire(1, 1);
	a.setCellStatus(1, 2, HAS_POISON);
	while (a.getCellStatus(1, 2) == HAS_POISON)
		a.moveVampires();
	a.moveVampires();
	a.history().display();
	cout << "====" << endl;
}