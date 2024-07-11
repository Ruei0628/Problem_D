#include "Net.h"
#include "Band.h"

constexpr double DX = 0.01;
constexpr double DY = 0.01;

bool checkIfIntersect(Band *source, Band *target, vector<Band*> &compairer, vector<Band*> &compairee) {
	for (Band *t : compairer) {
		for (Band *s : compairee) {
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

void mikami(Net &net, Chip &chip, vector<Band*> recordPath){
	cout << "Start mikami!" << endl;

	//step 1: initializaiotn
	Terminal source = net.TX;
	Terminal target = net.RXs[0]; // 待處理

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

	cout << "step 1 complete\n";

	while (1) {
		// step 2: check if intersect
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, CTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, OTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CTB, OSB)) break;

		cout << "step 2 complete\n";
		
		// step 3: copy CSB to OSB; copy CTB to OTB
		// current 的點要存回去 old
		OSB.insert(OSB.end(), CSB.begin(), CSB.end());
		OTB.insert(OTB.end(), CTB.begin(), CTB.end());

		// current 的資料應該不能刪掉，因為還要 extend
		// 只是在這之後(、被清除之前)調用 current bands 應該只能 const *
		cout << "step 3 complete\n";

		// step 4. 生成與 current bands 垂直的 extendedProbes，並先把他們暫存在一個 vector 裡面
		// 要分成 from source 跟 from target
		vector<Band*> ESB; // stands for extended source bands
		vector<Band*> ETB; // stands for extended target bands
		// 這些生成出來的 bands 的 level 要 +1

		for (Band *b : CSB) { // 來自 source
			vector<CoveredRange> coverageRight = b->generateCoveredRanges(edges, 1);
			vector<CoveredRange> coverageLeft = b->generateCoveredRanges(edges, 0);

			vector<Band*> tempESB = b->mergeCoveredRanges(coverageLeft, coverageRight);
			vector<Band*> ESB;
			for (Band *esb : tempESB) {
				if (esb->alreadyExist(OSB)) continue;
				if (esb->alreadyExist(CSB)) continue;
				if (esb->alreadyExist(ESB)) continue;
				ETB.push_back(esb);
			}
		}

		for (Band *b : CTB) { // 來自 target
			vector<CoveredRange> coverageRight = b->generateCoveredRanges(edges, 1);
			vector<CoveredRange> coverageLeft = b->generateCoveredRanges(edges, 0);

			vector<Band*> tempETB = b->mergeCoveredRanges(coverageLeft, coverageRight);
			vector<Band*> ETB;
			for (Band *etb : tempETB) {
				if (etb->alreadyExist(OTB)) continue;
				if (etb->alreadyExist(CTB)) continue;
				if (etb->alreadyExist(ETB)) continue;
				ETB.push_back(etb);
			}
		}

		// 因此我們現在獲得了全部的下一個 level 的 bands (在 extendedProbes 裡)
		CSB.swap(ESB);
		ESB.clear();
		CTB.swap(ETB);
		ETB.clear();

		cout << "step 4 complete\n";
	}
	
	// Step 5: Backtrace
	cout << "Path found!" << endl;
	vector<const Band*> path;

	// Backtrace from source Band
	const Band* band = sourceBandsBackTrace;
	while (band) {
		path.push_back(band);
		band = band->parent; // mistake
	}

	reverse(path.begin(), path.end()); // Reverse to get path from source to target

	// Backtrace from target Band
	band = targetBandsBackTrace;
	while (band) {
		path.push_back(band);
		band = band->parent; // mistake
	}

	// Clean up dynamically allocated memory
	delete sourceBandsBackTrace;
	delete targetBandsBackTrace;

	// Print the path
	cout << "len(Path): " << path.size() << endl;
    if (path.empty()) {
        cout << "Path is empty." << endl;
    } else {
        cout << "Path:" << endl;
        for (Band const *p : path) {
			cout << p->x.min << ", " << p->y.min << endl;
		} 
    }
}