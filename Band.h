#ifndef BAND_H_INCLUDED
#define BAND_H_INCLUDED

#include "Chip.h"
#include "Net.h"

class Band : public enable_shared_from_this<Band> {
public: 
    Band() {}
    Band(Pair, Pair, int, shared_ptr<Band>);
    Band(Terminal, bool, const vector<Edge>&);
    string terminalName = "";

    shared_ptr<Band> parent = nullptr;
    int level = 0;
    bool direction_isX;
    bool toExtend_isX() { return !direction_isX; }

    Pair x, y;
    Pair directionPair() { 
        return direction_isX ? x : y;
    }
    Pair extendedPair() {
        return toExtend_isX() ? x : y;
    }

    // member functions
    Pair directionPair(const vector<Edge> &edges, Point coord) const;
    bool intersected(const shared_ptr<Band> &other) const;

    shared_ptr<Band> extendBand(Pair, Pair);
    vector<Edge> generateCoveredRanges(vector<Edge> &edges, bool right);
    void addSource(const Edge &edge, vector<Pair> &uncovered, vector<Edge> &covered);
    vector<unique_ptr<Band>> mergeCoveredRanges(const vector<Edge> &left, const vector<Edge> &right);

    bool operator <=(const shared_ptr<Band> &other) const;
    friend ostream& operator<<(ostream& os, const Band& b);
    bool alreadyExist(const vector<shared_ptr<Band>> &bands);
};

#endif // BAND_H_INCLUDED