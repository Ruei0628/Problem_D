#include <iostream>
#include <vector>
#include <algorithm>
#include<set>

using namespace std;

class Pair {
public:
    double min, max;

    Pair() : min(0), max(0) {}
    Pair(double Min, double Max) {
        this->min = std::min(Min, Max);
        this->max = std::max(Min, Max);
    }

    void operator=(Pair const &other) {
        this->min = other.min;
        this->max = other.max;
    }
};

struct CoveredRange {
    Pair range;
    double parameter;

    CoveredRange(Pair r, double p) : range(r), parameter(p) {}
};

class RangeCoverage {
private:
    Pair target;
    vector<Pair> uncovered;

public:
    vector<CoveredRange> covered;
    vector<double> splitPoints;

    RangeCoverage(Pair target) : target(target) {
        uncovered.push_back(target);
        splitPoints = {target.min, target.max};
    }

    void addSource(Pair source, double parameter) {
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
                newCovered.push_back(CoveredRange(uncoveredRange, parameter));
            } else if (source.max <= uncoveredRange.min || source.min >= uncoveredRange.max) {
                // This uncovered range is not affected
                newUncovered.push_back(uncoveredRange);
            } else {
                // Partial overlap
                if (source.min > uncoveredRange.min) {
                    newUncovered.push_back(Pair(uncoveredRange.min, source.min));
                }
                if (source.max < uncoveredRange.max) {
                    newUncovered.push_back(Pair(source.max, uncoveredRange.max));
                }
                newCovered.push_back(CoveredRange(Pair(max(source.min, uncoveredRange.min), 
                                                       min(source.max, uncoveredRange.max)), 
                                                  parameter));
            }
        }

        uncovered = newUncovered;
        for (const auto& range : newCovered) {
            covered.push_back(range);
            splitPoints.push_back(range.range.min);
            splitPoints.push_back(range.range.max);
        }
    }

    bool isFullyCovered() const {
        return uncovered.empty();
    }

    void sortSplitPoints() {
        sort(splitPoints.begin(), splitPoints.end());
        splitPoints.erase(unique(splitPoints.begin(), splitPoints.end()), splitPoints.end());

        for (size_t i = 0; i < splitPoints.size(); ++i) {
            cout << splitPoints[i];
            if (i < splitPoints.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }

    void printCoveredRanges() const {
        for (const auto& range : covered) {
            cout << "CoveredRange(Pair(" << range.range.min << ", " << range.range.max 
                 << "), " << range.parameter << ")" << endl;
        }
    }
};

int main() {
    vector<CoveredRange> left = {  
        CoveredRange(Pair(0, 1), 3),
        CoveredRange(Pair(1, 6), 1),       
        CoveredRange(Pair(6, 7), 5),
        CoveredRange(Pair(7, 9), 2),     
        CoveredRange(Pair(9, 11), 6) 
    };

    vector<CoveredRange> right = {
        CoveredRange(Pair(0, 3), 11),
        CoveredRange(Pair(3, 5), 12),       
        CoveredRange(Pair(5, 7), 13),       
        CoveredRange(Pair(7, 9), 15),       
        CoveredRange(Pair(9, 11), 16)
    };

    // Sort the left and right vectors by their range.min

    vector<Band> result = mergeCoveredRanges(left, right);

    cout << "Resulting Bands:" << endl;
    printBands(result);

    return 0;
}