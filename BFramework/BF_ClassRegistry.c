#include "BF_Class.h"

#include "BCore/Memory/BC_Memory.h"
#include "BCore/Thread/BC_Threads.h"

#include <stddef.h>
#include <string.h>

// =========================================================
// MARK: Configuration
// =========================================================

// Initial segment size (must be power of 2)
// This is the size of segment 0. Each subsequent segment doubles in size.
#define BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE 64

// Maximum number of segments
// Total capacity = INITIAL_SIZE * (2^MAX_SEGMENTS - 1)
// Examples:
//   INITIAL=64, MAX=8  -> 64 * (256-1) = 16,320 classes
//   INITIAL=32, MAX=8  -> 32 * (256-1) = 8,160 classes
//   INITIAL=128, MAX=8 -> 128 * (256-1) = 32,640 classes
//   INITIAL=64, MAX=10 -> 64 * (1024-1) = 65,472 classes
#define BC_CLASS_REGISTRY_MAX_SEGMENTS 10

// =========================================================
// MARK: State
// =========================================================

static struct {
	BC_SPINLOCK_MAYBE(lock)
	BF_Class** segments[BC_CLASS_REGISTRY_MAX_SEGMENTS];
	BF_ClassId segment_count;
	BF_ClassId total_classes;
} gClassRegistryState;

// =========================================================
// MARK: Forwards
// =========================================================

static inline BF_ClassId PRIV_GetSegmentIndex(BF_ClassId class_index);
static inline BF_ClassId PRIV_GetSegmentOffset(BF_ClassId class_index);
static inline BF_ClassId PRIV_GetSegmentSize(BF_ClassId segment_index);

// =========================================================
// MARK: Initialization
// =========================================================

void INTERNAL_BF_ClassRegistryInitialize(void) {
	BC_SpinlockInit(&gClassRegistryState.lock);
	memset(gClassRegistryState.segments, 0, sizeof(gClassRegistryState.segments));
	gClassRegistryState.segment_count = 0;
	gClassRegistryState.total_classes = 0;
}

void INTERNAL_BF_ClassRegistryDeinitialize(void) {
	BC_SpinlockLock(&gClassRegistryState.lock);

	// Free all allocated segments
	for (BF_ClassId i = 0; i < gClassRegistryState.segment_count; i++) {
		if (gClassRegistryState.segments[i]) {
			BC_Free(gClassRegistryState.segments[i]);
			gClassRegistryState.segments[i] = NULL;
		}
	}

	gClassRegistryState.segment_count = 0;
	gClassRegistryState.total_classes = 0;

	BC_SpinlockUnlock(&gClassRegistryState.lock);
	BC_SpinlockDestroy(&gClassRegistryState.lock);
}

// =========================================================
// MARK: Class Public
// =========================================================

BF_ClassId BF_ClassRegistryGetCount(void) {
	return gClassRegistryState.total_classes;
}

BF_ClassId BF_ClassRegistryInsert(BF_Class* cls) {
	BC_SpinlockLock(&gClassRegistryState.lock);

	// Check if we need to allocate a new segment
	const uint32_t current_index = gClassRegistryState.total_classes;
	const uint32_t required_segment = PRIV_GetSegmentIndex(current_index);

	// Allocate segments as needed
	while (gClassRegistryState.segment_count <= required_segment) {
		if (gClassRegistryState.segment_count >= BC_CLASS_REGISTRY_MAX_SEGMENTS) {
			BC_SpinlockUnlock(&gClassRegistryState.lock);
			return BF_CLASS_ID_INVALID; // gClassRegistryState is full
		}

		const uint32_t segment_size = PRIV_GetSegmentSize(gClassRegistryState.segment_count);
		gClassRegistryState.segments[gClassRegistryState.segment_count] =
			BC_Malloc(segment_size * sizeof(BF_Class*));

		if (!gClassRegistryState.segments[gClassRegistryState.segment_count]) {
			BC_SpinlockUnlock(&gClassRegistryState.lock);
			return BF_CLASS_ID_INVALID; // Allocation failed
		}

		// Zero out the new segment
		memset(gClassRegistryState.segments[gClassRegistryState.segment_count], 0, segment_size * sizeof(BF_Class*));

		gClassRegistryState.segment_count++;
	}

	// Store the class pointer in the registry
	const uint32_t segment = PRIV_GetSegmentIndex(current_index);
	const uint32_t offset = PRIV_GetSegmentOffset(current_index);

	gClassRegistryState.segments[segment][offset] = cls;
	gClassRegistryState.total_classes++;
	cls->id = current_index;
	BC_SpinlockUnlock(&gClassRegistryState.lock);

	return current_index;
}

BF_Class* BF_ClassIdGetRef(const BF_ClassId cid) {
	if (cid >= gClassRegistryState.total_classes) {
		return NULL;
	}

	const uint32_t segment = PRIV_GetSegmentIndex(cid);
	const uint32_t offset = PRIV_GetSegmentOffset(cid);

	if (segment >= gClassRegistryState.segment_count || !gClassRegistryState.segments[segment]) {
		return NULL;
	}

	return gClassRegistryState.segments[segment][offset];
}

BF_ClassId BF_DebugClassFindId(const BF_Class* cls) {
	if (cls == NULL) { return BF_CLASS_ID_INVALID; }
	BC_SpinlockLock(&gClassRegistryState.lock);

	// Slow Linear Search, only for debugging purposes
	for (uint32_t i = 0; i < gClassRegistryState.total_classes; i++) {
		if (BF_ClassIdGetRef(i) == cls) {
			BC_SpinlockUnlock(&gClassRegistryState.lock);
			return i;
		}
	}

	BC_SpinlockUnlock(&gClassRegistryState.lock);
	return BF_CLASS_ID_INVALID;
}

// =========================================================
// MARK: Internal
// =========================================================

static inline BF_ClassId PRIV_GetSegmentIndex(const BF_ClassId class_index) {
	// Segments grow exponentially:
	// Segment 0: indices [0, INITIAL_SIZE)
	// Segment 1: indices [INITIAL_SIZE, INITIAL_SIZE + INITIAL_SIZE*2)
	// Segment 2: indices [INITIAL_SIZE*(1+2), INITIAL_SIZE*(1+2+4))
	// etc.

	uint32_t cumulative = 0;
	for (uint32_t seg = 0; seg < BC_CLASS_REGISTRY_MAX_SEGMENTS; seg++) {
		const uint32_t seg_size = PRIV_GetSegmentSize(seg);
		if (class_index < cumulative + seg_size) {
			return seg;
		}
		cumulative += seg_size;
	}

	// Should never reach here if class_index is valid
	return BC_CLASS_REGISTRY_MAX_SEGMENTS - 1;
}

static inline BF_ClassId PRIV_GetSegmentOffset(const BF_ClassId class_index) {
	// Calculate cumulative capacity up to the segment containing class_index
	uint32_t cumulative = 0;
	const uint32_t segment = PRIV_GetSegmentIndex(class_index);

	for (uint32_t seg = 0; seg < segment; seg++) {
		cumulative += PRIV_GetSegmentSize(seg);
	}

	// Offset within the segment is the difference
	return class_index - cumulative;
}

static inline BF_ClassId PRIV_GetSegmentSize(const BF_ClassId segment_index) {
	return BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE << segment_index;
}
