#include "BF_Class.h"

#include "BCore/Memory/BC_Memory.h"
#include "BCore/Thread/BC_Threads.h"

#include "BObject/BO_String.h"

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
} PRIV_ClassRegistryState;

// =========================================================
// MARK: Forwards
// =========================================================

static inline BF_ClassId PRIV_GetSegmentIndex(BF_ClassId class_index);
static inline BF_ClassId PRIV_GetSegmentOffset(BF_ClassId class_index);
static inline BF_ClassId PRIV_GetSegmentSize(BF_ClassId segment_index);

// =========================================================
// MARK: Class Public
// =========================================================

BO_StringPooledRef BF_ClassIdName(const BF_ClassId cid) {
	return BO_StringPooled(BF_ClassIdGetRef(cid)->name);
}

BF_ClassId BF_ClassRegistryGetCount(void) {
	return PRIV_ClassRegistryState.total_classes;
}

BF_ClassId BF_ClassRegistryInsert(BF_Class* cls) {
	BC_SpinlockLock(&PRIV_ClassRegistryState.lock);

	// Check if we need to allocate a new segment
	const uint32_t current_index = PRIV_ClassRegistryState.total_classes;
	const uint32_t required_segment = PRIV_GetSegmentIndex(current_index);

	// Allocate segments as needed
	while (PRIV_ClassRegistryState.segment_count <= required_segment) {
		if (PRIV_ClassRegistryState.segment_count >= BC_CLASS_REGISTRY_MAX_SEGMENTS) {
			BC_SpinlockUnlock(&PRIV_ClassRegistryState.lock);
			return BF_CLASS_ID_INVALID; // gClassRegistryState is full
		}

		const uint32_t segment_size = PRIV_GetSegmentSize(PRIV_ClassRegistryState.segment_count);
		PRIV_ClassRegistryState.segments[PRIV_ClassRegistryState.segment_count] =
			BC_Malloc(segment_size * sizeof(BF_Class*));

		if (!PRIV_ClassRegistryState.segments[PRIV_ClassRegistryState.segment_count]) {
			BC_SpinlockUnlock(&PRIV_ClassRegistryState.lock);
			return BF_CLASS_ID_INVALID; // Allocation failed
		}

		// Zero out the new segment
		memset(PRIV_ClassRegistryState.segments[PRIV_ClassRegistryState.segment_count], 0, segment_size * sizeof(BF_Class*));

		PRIV_ClassRegistryState.segment_count++;
	}

	// Store the class pointer in the registry
	const uint32_t segment = PRIV_GetSegmentIndex(current_index);
	const uint32_t offset = PRIV_GetSegmentOffset(current_index);

	PRIV_ClassRegistryState.segments[segment][offset] = cls;
	PRIV_ClassRegistryState.total_classes++;
	cls->id = current_index;
	BC_SpinlockUnlock(&PRIV_ClassRegistryState.lock);

	return current_index;
}

BF_Class* BF_ClassIdGetRef(const BF_ClassId cid) {
	if (cid >= PRIV_ClassRegistryState.total_classes) {
		return NULL;
	}

	const uint32_t segment = PRIV_GetSegmentIndex(cid);
	const uint32_t offset = PRIV_GetSegmentOffset(cid);

	if (segment >= PRIV_ClassRegistryState.segment_count || !PRIV_ClassRegistryState.segments[segment]) {
		return NULL;
	}

	return PRIV_ClassRegistryState.segments[segment][offset];
}

BF_ClassId BF_DebugClassFindId(const BF_Class* cls) {
	if (cls == NULL) { return BF_CLASS_ID_INVALID; }
	BC_SpinlockLock(&PRIV_ClassRegistryState.lock);

	// Slow Linear Search, only for debugging purposes
	for (uint32_t i = 0; i < PRIV_ClassRegistryState.total_classes; i++) {
		if (BF_ClassIdGetRef(i) == cls) {
			BC_SpinlockUnlock(&PRIV_ClassRegistryState.lock);
			return i;
		}
	}

	BC_SpinlockUnlock(&PRIV_ClassRegistryState.lock);
	return BF_CLASS_ID_INVALID;
}

// =========================================================
// MARK: Private
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

// =========================================================
// MARK: Internal
// =========================================================

void INTERNAL_BF_ClassRegistryInitialize(void) {
	BC_SpinlockInit(&PRIV_ClassRegistryState.lock);
	memset(PRIV_ClassRegistryState.segments, 0, sizeof(PRIV_ClassRegistryState.segments));
	PRIV_ClassRegistryState.segment_count = 0;
	PRIV_ClassRegistryState.total_classes = 0;
}

void INTERNAL_BF_ClassRegistryDeinitialize(void) {
	BC_SpinlockLock(&PRIV_ClassRegistryState.lock);

	// Free all allocated segments
	for (BF_ClassId i = 0; i < PRIV_ClassRegistryState.segment_count; i++) {
		if (PRIV_ClassRegistryState.segments[i]) {
			BC_Free(PRIV_ClassRegistryState.segments[i]);
			PRIV_ClassRegistryState.segments[i] = NULL;
		}
	}

	PRIV_ClassRegistryState.segment_count = 0;
	PRIV_ClassRegistryState.total_classes = 0;

	BC_SpinlockUnlock(&PRIV_ClassRegistryState.lock);
	BC_SpinlockDestroy(&PRIV_ClassRegistryState.lock);
}