#include <iostream>
#include "Edge.h"

using namespace std;

int main() {
	Pair r(3, 8);
	Pair s(5, 6);
	if (s <= r) { cout << "s <= r" << endl; }
	double f = 3;
	Edge e(r,f);
	cout << e.fixed();
}