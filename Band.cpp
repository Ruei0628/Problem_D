#include "Band.h"
#include <vector>

bool debugInfoBand = 0;

Band::Band(Pair X, Pair Y, int Level, Band *Parent) : x(X), y(Y), level(Level), parent(Parent) {
	direction_isX = !Parent->direction_isX;
}

Band::Band(Terminal terminal, bool direction_isX, vector<Edge> const &edges) 
:terminalName(terminal.name), direction_isX(direction_isX), parent(nullptr) {
	if (direction_isX) {
		x = this->directionPair(edges, terminal.coord);
		y = Pair(terminal.coord.y, terminal.coord.y);
	} else {
		y = this->directionPair(edges, terminal.coord);
		x = Pair(terminal.coord.x, terminal.coord.x);
	}
}

Pair Band::directionPair(vector<Edge> const &OrderedEdges, Point coord) const {
	double min = 0, max = 0;
	double TerminalFixed = direction_isX ? coord.y : coord.x;
	double TerminalChange = direction_isX ? coord.x : coord.y;
	for (auto const &e : OrderedEdges) {
		if (direction_isX == e.isVertical() && e.inRange(TerminalFixed)) {
			if (e.block->is_feedthroughable) {
				// some functions to deal with through block net num
				// TODO
				e.block->through_block_net_num--;
				if (!e.block->through_block_net_num) e.block->is_feedthroughable = 0;
				continue;
			}
			if (e.block) { if (terminalName.compare(e.block->name) == 0) {
				// some functions to deal with through block net num
				// TODO
				continue;
			} }
			if (e.fixed() > TerminalChange) {
				max = e.fixed();
				break;
			}
			min = e.fixed();
		}
	}
	return Pair(min, max);
}

bool Band::intersected(Band const *other) const {
	bool x_intersect = !(x.min > other->x.max || x.max < other->x.min);
	bool y_intersect = !(y.min > other->y.max || y.max < other->y.min);
	if (x_intersect && y_intersect) {
		//cout << "intered" << endl;
		//cout << *this << endl << *other << endl;
		return 1;
	}
	return 0;
}

Band* Band::extendBand(Pair x, Pair y) { return new Band(x, y, level + 1, this); }

vector<Edge> Band::generateCoveredRanges(vector<Edge> &edges, bool right) {
    vector<Pair> uncovered;
    vector<Edge> covered;

	uncovered.push_back(this->directionPair());

	if (right) { // positive search
		if (debugInfoBand) cout << "  $ positive:\n";
		for (auto const &e : edges) {
			if (e.isVertical() == this->toExtend_isX() && e.fixed() >= this->extendedPair().max) { // verified V
				if (e.block->is_feedthroughable) {
					// some functions to deal with through block net num
					// TODO
					e.block->through_block_net_num--;
					if (!e.block->through_block_net_num) e.block->is_feedthroughable = 0;
					continue;
				}
				if (e.block) { if (terminalName.compare(e.block->name) == 0) {
					// some functions to deal with through block net num
					// TODO
					continue;
				} }
				addSource(e, uncovered, covered);
				if (uncovered.empty()) break;
			}
		}
	} else { // negative search
		if (debugInfoBand) cout << "  $ negative:\n";
		for (auto it = edges.rbegin(); it != edges.rend(); ++it) {
			auto const &e = *it;
			if (e.isVertical() == this->toExtend_isX() && e.fixed() <= this->extendedPair().min) { // verified V
				if (e.block->is_feedthroughable) {
					// some functions to deal with through block net num
					// TODO
					e.block->through_block_net_num--;
					if (!e.block->through_block_net_num) e.block->is_feedthroughable = 0;
					continue;
				}
				if (e.block) { if (terminalName.compare(e.block->name) == 0) {
					// some functions to deal with through block net num
					// TODO
					continue;
				} }
				addSource(e, uncovered, covered);
				if (uncovered.empty()) break;
			}
		}
	}
	sort(covered.begin(), covered.end(), [](const Edge& a, const Edge& b) { return a.ranged().min < b.ranged().min; });
	for (auto const &c : covered) {
		if (debugInfoBand) cout << "   - edge [" << c.first.x << ", " << c.second.x << "] [" << c.first.y << ", " << c.second.y << "]\n";
	}
	return covered;
}

void Band::addSource(const Edge &e, vector<Pair> &uncovered, vector<Edge> &covered) {
	Pair source = e.ranged();
	Pair target = this->directionPair();
	double fixed = e.fixed();

    if (source.max <= target.min || source.min >= target.max) {
        return;  // This source is completely outside the target range
    }
    // Clip the source to the target range
    source.min = max(source.min, target.min);
    source.max = min(source.max, target.max);

    vector<Pair> newUncovered;
    vector<Edge> newCovered;

    for (const auto& uncoveredRange : uncovered) {
        if (source.min <= uncoveredRange.min && source.max >= uncoveredRange.max) {
            // This uncovered range is completely covered
            newCovered.push_back(Edge(uncoveredRange, fixed));
        } else if (source.max <= uncoveredRange.min || source.min >= uncoveredRange.max) {
            // This uncovered range is not affected
            newUncovered.push_back(uncoveredRange);
        } else {
            // Partial overlap
            if (source.min > uncoveredRange.min) { newUncovered.push_back(Pair(uncoveredRange.min, source.min)); }
            if (source.max < uncoveredRange.max) { newUncovered.push_back(Pair(source.max, uncoveredRange.max)); }
            newCovered.push_back(Edge(Pair(max(source.min, uncoveredRange.min), min(source.max, uncoveredRange.max)), fixed));
        }
    }

    uncovered = newUncovered;
    for (const auto& range : newCovered) { covered.push_back(range); }
}

vector<Band*> Band::mergeCoveredRanges(const vector<Edge>& left, const vector<Edge>& right) {
    vector<Band*> result;
    size_t i = 0, j = 0;
    double currentMin = min(left[0].ranged().min, right[0].ranged().min);
    double leftParameter = -1, rightParameter = -1;

    while (i < left.size() || j < right.size()) {
        double nextSplitPoint = 9999;
        
        if (i < left.size()) { nextSplitPoint = min(nextSplitPoint, left[i].ranged().max); }
        if (j < right.size()) { nextSplitPoint = min(nextSplitPoint, right[j].ranged().max); }

        if (i < left.size() && left[i].ranged().min <= currentMin) { leftParameter = left[i].fixed(); }
        if (j < right.size() && right[j].ranged().min <= currentMin) { rightParameter = right[j].fixed(); }

        if (leftParameter != -1 && rightParameter != -1) {
			if (toExtend_isX()) { 
				// the extendedBand's direction is X
				result.push_back(extendBand(Pair(leftParameter, rightParameter), Pair(currentMin, nextSplitPoint)));
			} else { 
				// the extendedBand's direction is Y
				result.push_back(extendBand(Pair(currentMin, nextSplitPoint), Pair(leftParameter, rightParameter)));
			}
        }

        currentMin = nextSplitPoint;

        if (i < left.size() && left[i].ranged().max == currentMin) {
            leftParameter = -1;
            i++;
        }
        if (j < right.size() && right[j].ranged().max == currentMin) {
            rightParameter = -1;
            j++;
        }
    }

    return result;
}

bool Band::operator <=(Band* const &other) const {
	return this->x <= other->x && this->y <= other->y;
}

bool Band::alreadyExist(vector<Band*> bands) {
	for (Band* const &b : bands) {
		if (*this <= b) {
        	if (debugInfoBand) cout << " X.";
			return 1;
		}
	}
	return 0;
}
ostream& operator <<(ostream& os, const Band& band) {
    os << "x[" << band.x.min << ", " << band.x.max << "]\ty[" << band.y.min << ", " << band.y.max << "] " << band.level;
    return os;
}
