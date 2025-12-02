#ifndef BCRUNTIME_BCRANGE_H
#define BCRUNTIME_BCRANGE_H

#include "../Core/BCTypes.h"

#include <stdio.h>

typedef struct BCRange {
	size_t start;
	size_t length;
} BCRange;

static inline BCRange BCRangeMakeEnd(const size_t location, const size_t end_exclusive) {
	return (BCRange){location, end_exclusive - location};
}

static inline BCRange BCRangeMakeEndIncl(const size_t location, const size_t end_inclusive) {
	return (BCRange){location, end_inclusive - location + 1};
}

static inline size_t BCRangeEndExclusive(const BCRange range) {
	return range.start + range.length;
}

static inline size_t BCRangeEndInclusive(const BCRange range) {
	return range.start + range.length - 1;
}

static inline BC_bool BCRangeContains(const BCRange range, const size_t loc) {
	return (loc >= range.start) && (loc < BCRangeEndExclusive(range));
}

static inline BC_bool BCRangeEqual(const BCRange range1, const BCRange range2) {
	return (range1.start == range2.start) && (range1.length == range2.length);
}

static inline BCRange BCRangeIntersection(const BCRange range1, const BCRange range2) {
	const size_t maxLoc = (range1.start > range2.start) ? range1.start : range2.start;
	const size_t maxRange1 = BCRangeEndExclusive(range1);
	const size_t maxRange2 = BCRangeEndExclusive(range2);
	const size_t minEnd = (maxRange1 < maxRange2) ? maxRange1 : maxRange2;

	if (minEnd > maxLoc) {
		return (BCRange){maxLoc, minEnd - maxLoc};
	}

	return (BCRange){0, 0};
}

static inline BCRange BCRangeUnion(const BCRange range1, const BCRange range2) {
	if (range1.length == 0) return range2;
	if (range2.length == 0) return range1;

	const size_t minLoc = (range1.start < range2.start) ? range1.start : range2.start;
	const size_t maxRange1 = BCRangeEndExclusive(range1);
	const size_t maxRange2 = BCRangeEndExclusive(range2);
	const size_t maxEnd = (maxRange1 > maxRange2) ? maxRange1 : maxRange2;

	return (BCRange){minLoc, maxEnd - minLoc};
}

#endif // BCRUNTIME_BCRANGE_H
