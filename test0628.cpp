#include "Net.h"

int main () {
	Chip chip(0);
	Net net;
	//net.ParserAllNets(0, chip);
	chip.showAllZones();
	cout << "done.";
	return 0;
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ test0628.cpp Chip.cpp Block.cpp Net.cpp Region.cpp -o test0628} ; if ($?) { .\test0628 }