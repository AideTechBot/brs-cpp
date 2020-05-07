// brs.cpp : Defines the entry point for the application.
//

#define BRS_BRICKADIA_IMPLEMENTATION_H
#include "brs.h"
#include <iostream>

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;
	Color r(255, 0, 0, 0);
	Color g(0, 255, 0, 0);
	Color b(0, 0, 255, 0);
	Color a(0, 0, 0, 255);
	cout << r + b + g + a;
	return 0;
}
