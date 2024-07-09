#include <iostream>
#include <vector>
#include <algorithm>

class Pair {
public:
    Pair() : min(0), max(0) {}
    Pair(double Min, double Max) {
        this->min = std::min(Min, Max);
        this->max = std::max(Min, Max);
    }
    double min, max;
};

class RangeCoverage {
private:
    Pair target;
    std::vector<Pair> covered;
    std::vector<Pair> uncovered;

public:
    RangeCoverage(Pair target) : target(target) {
        uncovered.push_back(target);
    }

    void addSource(Pair source) {
        if (source.max <= target.min || source.min >= target.max) {
            return;  // This source is completely outside the target range
        }

        // Clip the source to the target range
        source.min = std::max(source.min, target.min);
        source.max = std::min(source.max, target.max);

        std::vector<Pair> newUncovered;
        std::vector<Pair> newCovered;

        for (const auto& uncoveredRange : uncovered) {
            if (source.min <= uncoveredRange.min && source.max >= uncoveredRange.max) {
                // This uncovered range is completely covered
                newCovered.push_back(uncoveredRange);
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
                newCovered.push_back(Pair(std::max(source.min, uncoveredRange.min), 
                                          std::min(source.max, uncoveredRange.max)));
            }
        }

        uncovered = newUncovered;
        for (const auto& range : newCovered) {
            covered.push_back(range);
        }
    }

    bool isFullyCovered() const {
        return uncovered.empty();
    }

    void printCoveredRanges() const {
        std::cout << "covered:" << std::endl;
        for (const auto& range : covered) {
            std::cout << " - Pair(" << range.min << ", " << range.max << ")" << std::endl;
        }
    }

    void printUncovered() const {
        std::cout << "uncovered:" << std::endl;
        for (const auto& range : uncovered) {
            std::cout << " + Pair(" << range.min << ", " << range.max << ")" << std::endl;
        }
    }

	void printSortedFinalCoverage() {
        std::sort(covered.begin(), covered.end(),
                  [](const Pair& a, const Pair& b) { return a.min < b.min; });

        std::cout << "final:" << std::endl;
        for (const auto& range : covered) {
            std::cout << " - Pair(" << range.min << ", " << range.max << ")" << std::endl;
        }
    }
};

int main() {
    Pair target(0, 11);
    RangeCoverage coverage(target);

    std::vector<Pair> sources = {
        Pair(10, 12),
        Pair(4, 13),
        Pair(2, 3),
        Pair(3.333, 3.888),
        Pair(0.256, 1.234),
        Pair(0, 11)
    };

    for (const auto& source : sources) {
        std::cout << "Pair(" << source.min << ", " << source.max << ") added" << std::endl;
        coverage.addSource(source);
        coverage.printCoveredRanges();
        coverage.printUncovered();
        std::cout << std::endl;

        if (coverage.isFullyCovered()) {
            std::cout << "fully covered" << std::endl;
            break;
        }
    }

    coverage.printSortedFinalCoverage();

    return 0;
}