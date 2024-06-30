#include "Band.h"

bool Band::intersected(Band const &other) const{
	bool x_intersect = (min_X < other.min_X && other.min_X < max_X) || (other.min_X < min_X && min_X < other.max_X);
	bool y_intersect = (min_Y < other.min_Y && other.min_Y < max_Y) || (other.min_Y < min_Y && min_Y < other.max_Y);
	return x_intersect && y_intersect;
}