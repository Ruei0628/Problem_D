#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

class Pair {
public:
    double min, max;
	int level;

    Pair() : min(0), max(0) {}
    Pair(double Min, double Max) {
        this->min = std::min(Min, Max);
        this->max = std::max(Min, Max);
    }

    void operator=(Pair const &other) {
        this->min = other.min;
        this->max = other.max;
    }

    bool operator ==(const Pair &other) const {
    	return (min == other.min && max == other.max);
    }
};

void test(){
	vector<Pair*> Old = {};
	vector<Pair*> Current;
	
	Current.push_back(new Pair(0, 0));
	double index = 0;
	int iter = 0;
	while(1) {
		cout << "iter: " << iter << endl;
		Old.insert(Old.end(), Current.begin(), Current.end());

		vector<Pair*> Extended;
		for(size_t i = 0; i<2; i++){
			vector<Pair*> tempExtended;
			tempExtended.push_back(new Pair(index, index));
			index++;
			tempExtended.push_back(new Pair(index, index));
			for (Pair* &te : tempExtended) {
				for (Pair* &c : Extended) {
					if ( te == c) {
						delete te;
						continue;
					}
				}
				for (Pair* &c : Current) {
					if ( te == c) {
						delete te;
						continue;
					}
				}
				Extended.push_back(te);
			}
		}

		for (Pair *b : Current) { delete b; }
		Current.clear();

		// 然後把ESB、ETB的資料存入CSB、CTB裡面
		Current = std::move(Extended);

		cout << "> Current:\n";
		for (auto const &pp : Current) cout << pp->min << ", " << pp->max << endl;
		cout << "> Old:\n";
		for (auto const &pp : Old) cout << pp->min << ", " << pp->max << endl;
		iter++;
		if (iter > 5) return;
	}
}

int main() {
	test();
    return 0;
}