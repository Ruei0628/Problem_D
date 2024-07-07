#include "Net.h"

class test {
public:
	test(int a, int b) : a(a), b(b) {
	}
	int a = 5;
	int b = 5;
};

int main () {
	Chip chip(0);
	Net net;
	net.ParserAllNets(0, chip);
	for (Net const &n : net.allNets) {
		if (n.HMFT_MUST_THROUGHs.size() || n.MUST_THROUGHs.size()) {
			n.showNetInfo();
		}
	}
	cout << "done.";
	return 0;
}
