#ifndef BCORE_RANGE_H
#define BCORE_RANGE_H

#include "BC_Types.h"

#include <stdio.h>

typedef struct BC_Range {
	size_t start;
	size_t length;
} BC_Range;

static inline BC_Range BC_RangeMakeEnd(const size_t location, const size_t end_exclusive) {
	return (BC_Range){location, end_exclusive - location};
}

static inline BC_Range BC_RangeMakeEndIncl(const size_t location, const size_t end_inclusive) {
	return (BC_Range){location, end_inclusive - location + 1};
}

static inline size_t BC_RangeEndExclusive(const BC_Range range) {
	return range.start + range.length;
}

static inline size_t BC_RangeEndInclusive(const BC_Range range) {
	return range.start + range.length - 1;
}

static inline BC_bool BC_RangeContains(const BC_Range range, const size_t loc) {
	return (loc >= range.start) && (loc < BC_RangeEndExclusive(range));
}

static inline BC_bool BC_RangeEqual(const BC_Range range1, const BC_Range range2) {
	return (range1.start == range2.start) && (range1.length == range2.length);
}

static inline BC_Range BC_RangeIntersection(const BC_Range range1, const BC_Range range2) {
	const size_t maxLoc = (range1.start > range2.start) ? range1.start : range2.start;
	const size_t maxRange1 = BC_RangeEndExclusive(range1);
	const size_t maxRange2 = BC_RangeEndExclusive(range2);
	const size_t minEnd = (maxRange1 < maxRange2) ? maxRange1 : maxRange2;

	if (minEnd > maxLoc) {
		return (BC_Range){maxLoc, minEnd - maxLoc};
	}

	return (BC_Range){0, 0};
}

static inline BC_Range BC_RangeUnion(const BC_Range range1, const BC_Range range2) {
	if (range1.length == 0) return range2;
	if (range2.length == 0) return range1;

	const size_t minLoc = (range1.start < range2.start) ? range1.start : range2.start;
	const size_t maxRange1 = BC_RangeEndExclusive(range1);
	const size_t maxRange2 = BC_RangeEndExclusive(range2);
	const size_t maxEnd = (maxRange1 > maxRange2) ? maxRange1 : maxRange2;

	return (BC_Range){minLoc, maxEnd - minLoc};
}

#endif // BCRUNTIME_BC_Range_H
