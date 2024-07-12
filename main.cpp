#include "Net.h"
#include "Band.h"

constexpr double DX = 0.01;
constexpr double DY = 0.01;

bool checkIfIntersect(Band *source, Band *target, vector<Band*> &compairer, vector<Band*> &compairee) {
	for (Band *s : compairer) {
		for (Band *t : compairee) {
			if (s->intersected(t)) {
				// which means path is found
				source = s;
				target = t;
				return 1;
			}
		}
	}
	return 0;
}

void printBands(const vector<Band*>& bands) {
    for (const auto& band : bands) {
        cout << " > [" << band->x.min << ", " << band->x.max << "]";
        cout << " [" << band->y.min << ", " << band->y.max << "] " << band->level << endl;
    }
}

void bandSearchAlgorithm(Net &net, Chip &chip, vector<Band*> &recordPath) {
	cout << "Start band search!" << endl;

	//step 1: initializaiotn
	Terminal source = net.TX;
	Terminal target = net.RXs[0]; // 待處理

	/*
	現在還沒寫的
	1. 現在search出來的東西會是一堆band，
	   要如何在band path中找到一條optimized net，
	   同時要注意net間距的問題
	2. must through還沒穿過
	3. 還沒run過
	*/

	vector<Band*> CSB; // stands for current source bands
	vector<Band*> OSB; // stands for old source bands
	vector<Band*> CTB; // stands for current target bands
	vector<Band*> OTB; // stands for old target bands

	vector<Edge*> &edges = chip.totEdge; // make it referenced

	// 把 TX 跟 RX 改成 Bands
	CSB.push_back(new Band(source, 1, edges));
	CSB.push_back(new Band(source, 0, edges));
	CTB.push_back(new Band(target, 1, edges));
	CTB.push_back(new Band(target, 0, edges));

	Band* sourceBandsBackTrace = nullptr;
	Band* targetBandsBackTrace = nullptr;

	//cout << "step 1 complete\n";

	int levelOfIteration = 0;
	while (1) {
		cout << "=== level Of Iteration: " << ++levelOfIteration << " ===\n";
		
		// step 2: check if intersect 
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, CTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, OTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, OSB, CTB)) break;
		
		//cout << "step 2 complete\n";

		// step 3: copy CSB to OSB; copy CTB to OTB
		// current 的點要存回去 old
		OSB.insert(OSB.end(), CSB.begin(), CSB.end());
		OTB.insert(OTB.end(), CTB.begin(), CTB.end());

		// current 的資料應該不能刪掉，因為還要 extend
		// 只是在這之後(、被清除之前)調用 current bands 應該只能 const *
		//cout << "step 3 complete\n";

		// step 4. 生成與 current bands 垂直的 extendedProbes，並先把他們暫存在一個 vector 裡面
		// 要分成 from source 跟 from target
		vector<Band*> ESB; // stands for extended source bands
		vector<Band*> ETB; // stands for extended target bands
		// 這些生成出來的 bands 的 level 要 +1

		cout << "< from source >\n";
		for (Band *b : CSB) { // 來自 source
        	cout << " # band [" << b->x.min << ", " << b->x.max << "]";
        	cout << " [" << b->y.min << ", " << b->y.max << "] " << endl;

			vector<Edge> coverageRight = b->generateCoveredRanges(edges, 1);
			vector<Edge> coverageLeft = b->generateCoveredRanges(edges, 0);

			vector<Band*> tempESB = b->mergeCoveredRanges(coverageLeft, coverageRight);
			for (Band *esb : tempESB) {
                cout << "  + adding [" << esb->x.min << ", " << esb->x.max << "] [" << esb->y.min << ", " << esb->y.max << "]";
				if (esb->alreadyExist(OSB) || esb->alreadyExist(CSB) || esb->alreadyExist(ESB)) {
					cout << endl;
					delete esb;
					continue;
				}
				ESB.push_back(esb);
				cout << endl;
			}
			cout << endl;
		}

		cout << "< from target >\n";
		for (Band *b : CTB) { // 來自 target
        	cout << " # band [" << b->x.min << ", " << b->x.max << "]";
        	cout << " [" << b->y.min << ", " << b->y.max << "] " << endl;

			vector<Edge> coverageRight = b->generateCoveredRanges(edges, 1);
			vector<Edge> coverageLeft = b->generateCoveredRanges(edges, 0);

			vector<Band*> tempETB = b->mergeCoveredRanges(coverageLeft, coverageRight);
			for (Band *etb : tempETB) {
                cout << "  + adding [" << etb->x.min << ", " << etb->x.max << "] [" << etb->y.min << ", " << etb->y.max << "]";
				if (etb->alreadyExist(OTB) || etb->alreadyExist(CTB) || etb->alreadyExist(ETB)) {
					cout << endl;
					delete etb;
					continue;
				}
				ETB.push_back(etb);
				cout << endl;
			}
			cout << endl;
		}

		// 因此我們現在獲得了全部的下一個 level 的 bands (在 extendedBands 裡)
		// 接下來要將CSB、CTB的資料刪除
		//for (Band *b : CSB) { delete b; }
		//for (Band *b : CTB) { delete b; }
		CSB.clear();
		CTB.clear();

		// 然後把ESB、ETB的資料存入CSB、CTB裡面
		CSB = std::move(ESB);
		CTB = std::move(ETB);

		cout << "> CSB:\n";
		printBands(CSB);
		cout << "> CTB:\n";
		printBands(CTB);
		cout << "> OSB:\n";
		printBands(OSB);
		cout << "> OTB:\n";
		printBands(OTB);

		//cout << "step 4 complete\n";
		cout << endl;
		if (levelOfIteration == 5) return;
	}

	// Step 5: Backtrace
	cout << "Path found!" << endl;

	// Backtrace from source Band
	Band* band = sourceBandsBackTrace;
	while (band) {
		recordPath.push_back(band);
		band = band->parent;
	}

	reverse(recordPath.begin(), recordPath.end()); // Reverse to get path from source to target

	// Backtrace from target Band
	band = targetBandsBackTrace;
	while (band) {
		recordPath.push_back(band);
		band = band->parent;
	}

	// Clean up dynamically allocated memory
	delete sourceBandsBackTrace;
	delete targetBandsBackTrace;

	// Print the path
	cout << "len(Path): " << recordPath.size() << endl;
    cout << "Path:" << endl;
    for (Band const *p : recordPath) {
		cout << p->x.min << ", " << p->y.min << endl;
	}
	
	for (Band *b : OSB) { delete b; }
	for (Band *b : CSB) { delete b; }
	for (Band *b : OTB) { delete b; }
	for (Band *b : CTB) { delete b; }
}

int main() {
	/*1*/

	Chip chip(0);
	chip.border = Point(33, 15);
	chip.totEdge.clear();

	chip.totEdge.push_back(new Edge(Point(0, 4), Point(4, 4)));
	chip.totEdge.push_back(new Edge(Point(4, 4), Point(4, 11)));
	chip.totEdge.push_back(new Edge(Point(4, 11), Point(7, 11)));
	chip.totEdge.push_back(new Edge(Point(7, 11), Point(7, 13)));
	chip.totEdge.push_back(new Edge(Point(23, 13), Point(7, 13)));
	chip.totEdge.push_back(new Edge(Point(23, 13), Point(23, 15)));

	chip.totEdge.push_back(new Edge(Point(9, 10), Point(9, 12)));
	chip.totEdge.push_back(new Edge(Point(9, 12), Point(20, 12)));
	chip.totEdge.push_back(new Edge(Point(20, 12), Point(20, 10)));
	chip.totEdge.push_back(new Edge(Point(9, 10), Point(20, 10)));

	chip.totEdge.push_back(new Edge(Point(11, 0), Point(11, 9)));
	chip.totEdge.push_back(new Edge(Point(11, 9), Point(17, 9)));
	chip.totEdge.push_back(new Edge(Point(17, 7), Point(17, 9)));
	chip.totEdge.push_back(new Edge(Point(14, 7), Point(17, 7)));
	chip.totEdge.push_back(new Edge(Point(14, 7), Point(14, 0)));

	chip.totEdge.push_back(new Edge(Point(17, 2), Point(17, 4)));
	chip.totEdge.push_back(new Edge(Point(17, 4), Point(22, 4)));
	chip.totEdge.push_back(new Edge(Point(22, 4), Point(22, 12)));
	chip.totEdge.push_back(new Edge(Point(22, 12), Point(25, 12)));
	chip.totEdge.push_back(new Edge(Point(25, 12), Point(25, 10)));
	chip.totEdge.push_back(new Edge(Point(25, 10), Point(28, 10)));
	chip.totEdge.push_back(new Edge(Point(28, 10), Point(28, 8)));
	chip.totEdge.push_back(new Edge(Point(28, 8), Point(25, 8)));
	chip.totEdge.push_back(new Edge(Point(25, 8), Point(25, 3)));
	chip.totEdge.push_back(new Edge(Point(25, 3), Point(22, 3)));
	chip.totEdge.push_back(new Edge(Point(22, 3), Point(22, 2)));
	chip.totEdge.push_back(new Edge(Point(22, 2), Point(17, 2)));

	chip.totEdge.push_back(new Edge(Point(24, 15), Point(24, 13)));
	chip.totEdge.push_back(new Edge(Point(24, 13), Point(30, 13)));
	chip.totEdge.push_back(new Edge(Point(30, 13), Point(30, 6)));
	chip.totEdge.push_back(new Edge(Point(30, 6), Point(27, 6)));
	chip.totEdge.push_back(new Edge(Point(27, 6), Point(27, 4)));
	chip.totEdge.push_back(new Edge(Point(27, 4), Point(30, 4)));
	chip.totEdge.push_back(new Edge(Point(30, 4), Point(30, 3)));
	chip.totEdge.push_back(new Edge(Point(30, 3), Point(33, 3)));

	// Edges: already have block Edges, here adding the chip border
	chip.totEdge.push_back(new Edge(Point(0, 0), Point(0, chip.border.y)));
	chip.totEdge.push_back(new Edge(Point(0, 0), Point(chip.border.x, 0)));
	chip.totEdge.push_back(new Edge(Point(0, chip.border.y), chip.border));
	chip.totEdge.push_back(new Edge(Point(chip.border.x, 0), chip.border));

	// make it ordered
	std::sort(chip.totEdge.begin(), chip.totEdge.end(), 
	[](const Edge* a, const Edge* b) { return a->fixed() < b->fixed(); });

	Terminal start("testS", Point(6, 5));
  	Terminal end("testT", Point(26, 3));
	Net n(start, end);
	
	vector<Band*> record;
	bandSearchAlgorithm(n, chip, record);

	cout << "done" << endl;
	return 0;

	/*2*/

	Net net;
	net.ParserAllNets(0, chip);
	//vector<Band*> record;

	int index = 0;
	for (Net n : net.allNets) {
		index++;
		bandSearchAlgorithm(n, chip, record);
		if (index > 3) break;
	}

	cout << "done" << endl;
	return 0;
}

/*
cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Band.cpp Chip.cpp Block.cpp Net.cpp Region.cpp -o main} ; if ($?) { .\main }

cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ test0628.cpp Chip.cpp Block.cpp Net.cpp Probe.cpp Region.cpp  Edge.cpp -o test0628} ; if ($?) { .\main }

cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Net.cpp -o main} ; if ($?) { .\main }
*/