#include "Band.h"
#include <vector>

Band::Band(Pair X, Pair Y, int Level, Band *Parent) : x(X), y(Y), level(Level), parent(Parent) {
	// uses in extendBand
}

Band::Band(Terminal terminal, bool direction_isX, vector<Edge*> const &edges)
		  :zoneName(terminal.name), direction_isX(direction_isX), parent(nullptr) {
	if (direction_isX) {
		x = this->directionPair(edges, terminal.coord);
		y = Pair(terminal.coord.y, terminal.coord.y);
	} else {
		y = this->directionPair(edges, terminal.coord);
		x = Pair(terminal.coord.x, terminal.coord.x);
	}
}

Pair Band::directionPair(vector<Edge*> const OrderedEdges, Point coord) const {
	double min = 0, max = 0;
	for (Edge *w : OrderedEdges) {
		double TerminalFixed = direction_isX ? coord.y : coord.x;
		double TerminalChange = direction_isX ? coord.x : coord.y;
		if (direction_isX == w->isVertical() && w->inRange(TerminalFixed)) {
			if (w->fixed() > TerminalChange) {
				max = w->fixed();
				break;
			}
			min = w->fixed();
		}
	}
	return Pair(min, max);
}

bool Band::intersected(Band const *other) const {
	//cout << " * this: [" << x.min << ", " << x.max << ", " << y.min << ", " << y.max << "]\t";
	//cout << "that: [" << other->x.min << ", " << other->x.max << ", " << other->y.min << ", " << other->y.max << "]\n";
	bool x_intersect = !(x.min > other->x.max || x.max < other->x.min);
	bool y_intersect = !(y.min > other->y.max || y.max < other->y.min);
	if (x_intersect && y_intersect) {
		cout << "intered" << endl;
		return 1;
	}
	return 0;
}

Band* Band::extendBand(Pair x, Pair y) { return new Band(x, y, level + 1, this); }

vector<CoveredRange> Band::generateCoveredRanges(vector<Edge*> &edges, bool right) {
    vector<Pair> uncovered;
    vector<CoveredRange> covered;

	uncovered.push_back(this->directionPair());

	if (right) { // positive search
		cout << "(+)positive:\n";
		for (Edge *e : edges) {
			if (e->isVertical() == this->toExtend_isX() && e->fixed() >= this->extendedPair().max) { // verified V
				addSource(e, uncovered, covered);
				if (uncovered.empty()) break;
			}
		}
	} else { // negative search
		cout << "(-)negative:\n";
		for (auto it = edges.rbegin(); it != edges.rend(); ++it) {
			Edge *e = *it;
			if (e->isVertical() == this->toExtend_isX() && e->fixed() <= this->extendedPair().min) { // verified V
				addSource(e, uncovered, covered);
				if (uncovered.empty()) break;
			}
		}
	}
	sort(covered.begin(), covered.end(), [](const CoveredRange& a, const CoveredRange& b) { return a.range.min < b.range.min; });
	for (const auto& cr : covered) {
        cout << "   > Pair(" << cr.range.min << ", " << cr.range.max << "), " << cr.fixed << endl;
    }
	return covered;
}

void Band::addSource(Edge *e, vector<Pair> &uncovered, vector<CoveredRange> &covered) {
	Pair source = e->ranged();
	Pair target = this->directionPair();
	double fixed = e->fixed();

    if (source.max <= target.min || source.min >= target.max) {
        return;  // This source is completely outside the target range
    }
    // Clip the source to the target range
    source.min = max(source.min, target.min);
    source.max = min(source.max, target.max);

    vector<Pair> newUncovered;
    vector<CoveredRange> newCovered;

    for (const auto& uncoveredRange : uncovered) {
        if (source.min <= uncoveredRange.min && source.max >= uncoveredRange.max) {
            // This uncovered range is completely covered
            newCovered.push_back(CoveredRange(uncoveredRange, fixed));
        } else if (source.max <= uncoveredRange.min || source.min >= uncoveredRange.max) {
            // This uncovered range is not affected
            newUncovered.push_back(uncoveredRange);
        } else {
            // Partial overlap
            if (source.min > uncoveredRange.min) { newUncovered.push_back(Pair(uncoveredRange.min, source.min)); }
            if (source.max < uncoveredRange.max) { newUncovered.push_back(Pair(source.max, uncoveredRange.max)); }
            newCovered.push_back(CoveredRange(Pair(max(source.min, uncoveredRange.min), min(source.max, uncoveredRange.max)), fixed));
        }
    }

    uncovered = newUncovered;
    for (const auto& range : newCovered) { covered.push_back(range); }
}

vector<Band*> Band::mergeCoveredRanges(const vector<CoveredRange>& left, const vector<CoveredRange>& right) {
    vector<Band*> result;
    size_t i = 0, j = 0;
    double currentMin = min(left[0].range.min, right[0].range.min);
    double leftParameter = -1, rightParameter = -1;

    while (i < left.size() || j < right.size()) {
        double nextSplitPoint = 9999;
        
        if (i < left.size()) { nextSplitPoint = min(nextSplitPoint, left[i].range.max); }
        if (j < right.size()) { nextSplitPoint = min(nextSplitPoint, right[j].range.max); }

        if (i < left.size() && left[i].range.min <= currentMin) { leftParameter = left[i].fixed; }
        if (j < right.size() && right[j].range.min <= currentMin) { rightParameter = right[j].fixed; }

        if (leftParameter != -1 && rightParameter != -1) {
			if (toExtend_isX()) { // the extendedBand's direction is X
				result.push_back(extendBand(Pair(leftParameter, rightParameter), Pair(currentMin, nextSplitPoint)));
			} else { // the extendedBand's direction is Y
				result.push_back(extendBand(Pair(currentMin, nextSplitPoint), Pair(leftParameter, rightParameter)));
			}
        }

        currentMin = nextSplitPoint;

        if (i < left.size() && left[i].range.max == currentMin) {
            leftParameter = -1;
            i++;
        }
        if (j < right.size() && right[j].range.max == currentMin) {
            rightParameter = -1;
            j++;
        }
    }

    return result;
}

bool Band::operator ==(Band* const other) const {
	return this->x == other->x && this->y == other->y;
}

bool Band::alreadyExist(vector<Band*> bands) {
	for (Band* const b : bands) {
		if (*this == b) {
        	cout << "x: [" << b->x.min << ", " << b->x.max << "]";
        	cout << " y: [" << b->y.min << ", " << b->y.max << "]" << endl;
			return 1;
		}
	}
	return 0;
}
