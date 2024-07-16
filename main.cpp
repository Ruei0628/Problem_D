#include "Net.h"
#include "Band.h"
#include <iomanip>
#include <memory>
#include <string>
#include <vector>

constexpr double DX = 0.01;
constexpr double DY = 0.01;

bool debugInfo = 0;

bool checkIfIntersect(Band *&source, Band *&target, vector<Band*> &vec_source, vector<Band*> &vec_target) {
	for (Band *s : vec_source) {
		for (Band *t : vec_target) {
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
    for (const auto& band : bands) { cout << " > " << *band << endl; }
}

void bandSearchAlgorithm(Net &net, Chip &chip, vector<Band*> &recordPath) {
	if (debugInfo) cout << "Start band search!" << endl;

	//step 1: initializaion
	Terminal source = net.TX;
	Terminal target = net.RXs[0]; // waited to be fix

	/*============= not write yet =============
	1. now search output is vector of bands
	   how to find optimized net from many band path
	   and also the spacing of nets
	2. must through not done yet
	3. when a net feed throughs a block
	   that block's through_block_net_num's gonna decrease
	   but not done yet
	============= not write yet =============*/

	vector<Band*> CSB; // stands for current source bands
	vector<Band*> OSB; // stands for old source bands
	vector<Band*> CTB; // stands for current target bands
	vector<Band*> OTB; // stands for old target bands

	vector<Edge> &edges = chip.allEdges; // make it referenced

	// change TX and RX into Bands
	CSB.push_back(new Band(source, 1, edges));
	CSB.push_back(new Band(source, 0, edges));
	CTB.push_back(new Band(target, 1, edges));
	CTB.push_back(new Band(target, 0, edges));
	if (debugInfo) cout << "2" << endl;

	Band* sourceBandsBackTrace = nullptr;
	Band* targetBandsBackTrace = nullptr;
	if (debugInfo) cout << "3" << endl;

	if (debugInfo) cout << "step 1 complete\n";

	int levelOfIteration = 0;
	while (1) {
		if (debugInfo) cout << "=== level Of Iteration: " << ++levelOfIteration << " ===\n";

		// step 2: check if intersect
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, CTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, OTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, OSB, CTB)) break;

		if (debugInfo) cout << "step 2 complete\n";

		// step 3: copy CSB to OSB; copy CTB to OTB
		// current bands stores to old
		OSB.insert(OSB.end(), CSB.begin(), CSB.end());
		OTB.insert(OTB.end(), CTB.begin(), CTB.end());

		// current data can't clear now, because used in extend
		// but from now on, current bands should read only
		if (debugInfo) cout << "step 3 complete\n";

		// step 4. generate extended bands that perpendicular to current bands
		// store them in a temporary vector
		// distinguished from source and from target
		vector<Band*> ESB; // stands for extended source bands
		vector<Band*> ETB; // stands for extended target bands
		// these bands' level are +1

		if (debugInfo) cout << "< from source >\n";
		for (Band *b : CSB) { // from source
        	if (debugInfo) cout << " # band " << *b << endl;

			vector<Edge> coverageRight = b->generateCoveredRanges(edges, 1);
			vector<Edge> coverageLeft = b->generateCoveredRanges(edges, 0);

			vector<Band*> tempESB = b->mergeCoveredRanges(coverageLeft, coverageRight);
			for (Band *esb : tempESB) {
                if (debugInfo) cout << "  + adding " << *esb;
				if (esb->alreadyExist(OSB) || esb->alreadyExist(CSB) || esb->alreadyExist(ESB)) {
					if (debugInfo) cout << endl;
					delete esb;
					continue;
				}
				ESB.push_back(esb);
				if (debugInfo) cout << endl;
			}
			if (debugInfo) cout << endl;
		}

		if (debugInfo) cout << "< from target >\n";
		for (Band *b : CTB) { // from target
        	if (debugInfo) cout << " # band " << *b << endl;

			vector<Edge> coverageRight = b->generateCoveredRanges(edges, 1);
			vector<Edge> coverageLeft = b->generateCoveredRanges(edges, 0);

			vector<Band*> tempETB = b->mergeCoveredRanges(coverageLeft, coverageRight);
			for (Band *etb : tempETB) {
                if (debugInfo) cout << "  + adding " << *etb;
				if (etb->alreadyExist(OTB) || etb->alreadyExist(CTB) || etb->alreadyExist(ETB)) {
					if (debugInfo) cout << endl;
					delete etb;
					continue;
				}
				ETB.push_back(etb);
				if (debugInfo) cout << endl;
			}
			if (debugInfo) cout << endl;
		}

		// now we obtain all next level bands inside extendedBands
		// now clears CSB, CTB
		CSB.clear();
		CTB.clear();

		// moves ESB, ETB's data into CSB, CTB
		CSB = std::move(ESB);
		CTB = std::move(ETB);

		if (debugInfo) {
		cout << "> CSB:\n";
		printBands(CSB);
		cout << "> CTB:\n";
		printBands(CTB);
		cout << "> OSB:\n";
		printBands(OSB);
		cout << "> OTB:\n";
		printBands(OTB);
		cout << "step 4 complete\n\n";
		}

		if (levelOfIteration > 10) {
			cout << "can't find :(\n";
			return;
		}
	}

	// Step 5: Backtrace
	cout << "Path found!" << endl;

	// Backtrace from source Band
	while (sourceBandsBackTrace) {
		recordPath.push_back(sourceBandsBackTrace);
		sourceBandsBackTrace = sourceBandsBackTrace->parent;
	}

	reverse(recordPath.begin(), recordPath.end()); // Reverse to get path from source to target

	// Backtrace from target Band
	while (targetBandsBackTrace) {
		recordPath.push_back(targetBandsBackTrace);
		targetBandsBackTrace = targetBandsBackTrace->parent;
	}

	// Clean up dynamically allocated memory
	delete sourceBandsBackTrace;
	delete targetBandsBackTrace;

	// Print the path
	cout << "length of Path: " << recordPath.size() << endl;
    cout << "Path:" << endl;
    printBands(recordPath);

	for (Band *b : OSB) { delete b; }
	for (Band *b : CSB) { delete b; }
	for (Band *b : OTB) { delete b; }
	for (Band *b : CTB) { delete b; }
	return;
}

int main() {

	Chip chip(4);
	vector<Band*> record;

	//cout << fixed << setprecision(3);

	Net net;
	net.ParserAllNets(4, chip);

	int index = 0;
	for (Net n : net.allNets) {
		index++;
		if (index == 10) break;
		Terminal source = n.TX;
		for (Terminal target : n.RXs) {
			Net sole(source, target);
			cout << "(" << sole.TX.coord.x << ", " << sole.TX.coord.y << ") (" << sole.RXs[0].coord.x << ", " << sole.RXs[0].coord.y << ")\n";
			bandSearchAlgorithm(sole, chip, record);
		}
	}

	cout << "done" << endl;
    return 0; /*---------------------------------------------------------*/

	// customed test data
	Chip tesCh;
	for (int i = 0; i < 6; i++) { tesCh.allBlocks.push_back(make_unique<Block>("block" + to_string(i))); }
	tesCh.allBlocks.push_back(make_unique<Block>("testS"));
	tesCh.allBlocks.push_back(make_unique<Block>("testT"));

	// regular borders
	tesCh.allBlocks[0]->vertices = { Point(11, 0), Point(11, 9), Point(17, 9), Point(17, 7), Point(14, 7), Point(14, 0) };
	tesCh.allBlocks[1]->vertices = { Point(9, 10), Point(9, 12), Point(20, 12), Point(20, 10) };
	tesCh.allBlocks[2]->vertices = { Point(17, 2), Point(17, 4), Point(22, 4), Point(22, 12), Point(25, 12), Point(25, 10), Point(28, 10), Point(28, 8), Point(25, 8), Point(25, 3), Point(22, 3), Point(22, 2) };
	tesCh.allBlocks[3]->vertices = { Point(0, 4), Point(4, 4), Point (4, 11), Point(7, 11), Point(7, 13), Point(23, 13), Point(23, 15), Point(0, 15) };
	tesCh.allBlocks[4]->vertices = { Point(24, 13), Point(24, 15), Point(33, 15), Point(33, 3), Point(30, 3), Point(30, 4), Point(27, 4), Point(27, 6), Point(30, 6), Point(30, 13) };
	tesCh.allBlocks[5]->vertices = { Point(0, 0), Point(0, 15), Point(33, 15), Point(33, 0) };

	// the block contains terminal
	tesCh.allBlocks[6]->vertices = { Point(4.9, 3.9), Point(4.9, 6.1), Point(7.1, 6.1), Point(7.1, 3.9) }; // testS
	tesCh.allBlocks[7]->vertices = { Point(25.5, 2), Point(25.5, 3.3), Point(28.8, 3.3), Point(28.8, 2) }; // testT
	for (auto &b : tesCh.allBlocks) { b->verticesToEdges(); for (auto &e : b->edges) { tesCh.allEdges.push_back(e); } }

	// make it ordered
	std::sort(tesCh.allEdges.begin(), tesCh.allEdges.end(), [](const auto& a, const auto& b) { return a.fixed() < b.fixed(); });

	Terminal start("testS", Point(6, 5));
  	Terminal end("testT", Point(26, 3));
	Net n(start, end);

	bandSearchAlgorithm(n, tesCh, record);

	cout << "done" << endl;
    return 0; /*---------------------------------------------------------*/

    // this tests if edges are linked to block, and is well!
	for (auto e : chip.allEdges) {
		if (e.block) {
			cout << e.block->name << ": ";
			if (e.block->name.length() == 7) { cout << " "; }
		}
		else { cout << "_BORDER_: "; }
		cout << e.isVertical();
		cout << " (" << e.fixed() << ")\t(" << e.ranged().min << ", " << e.ranged().max << ")\n";
	}
    return 0; /*---------------------------------------------------------*/
}

/*
cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Band.cpp Chip.cpp Block.cpp Net.cpp Region.cpp -o main} ; if ($?) { .\main }

cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ test0628.cpp Chip.cpp Block.cpp Net.cpp Probe.cpp Region.cpp  Edge.cpp -o test0628} ; if ($?) { .\main }

cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Net.cpp -o main} ; if ($?) { .\main }
*/