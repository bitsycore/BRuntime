#include "BCClassRegistry.h"

#include "BCClass.h"
#include "Utilities/BCMemory.h"
#include "Utilities/BCThreads.h"

#include <stddef.h>
#include <string.h>

// =========================================================
// MARK: Configuration
// =========================================================

// Initial segment size (must be power of 2)
#define BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE 64

// Maximum number of segments
// 8 segments = 64 + ... + 8192 = 16,320 classes
#define BC_CLASS_REGISTRY_MAX_SEGMENTS 8

// =========================================================
// MARK: State
// =========================================================

static struct {
	BC_MUTEX_MAYBE(lock)
	BCClass** segments[BC_CLASS_REGISTRY_MAX_SEGMENTS];
	uint32_t segment_count;
	uint32_t total_classes;
} gClassRegistryState;

// =========================================================
// MARK: Forwards
// =========================================================

static inline uint32_t GetSegmentIndex(uint32_t class_index);
static inline uint32_t GetSegmentOffset(uint32_t class_index);
static inline uint32_t GetSegmentSize(uint32_t segment_index) ;

// =========================================================
// MARK: Initialization
// =========================================================

void ___BCINTERNAL___ClassRegistryInitialize(void) {
	BCMutexInit(&gClassRegistryState.lock);
	memset(gClassRegistryState.segments, 0, sizeof(gClassRegistryState.segments));
	gClassRegistryState.segment_count = 0;
	gClassRegistryState.total_classes = 0;
}

void ___BCINTERNAL___ClassRegistryDeinitialize(void) {
	BCMutexLock(&gClassRegistryState.lock);

	// Free all allocated segments
	for (uint32_t i = 0; i < gClassRegistryState.segment_count; i++) {
		if (gClassRegistryState.segments[i]) {
			BCFree(gClassRegistryState.segments[i]);
			gClassRegistryState.segments[i] = NULL;
		}
	}

	gClassRegistryState.segment_count = 0;
	gClassRegistryState.total_classes = 0;

	BCMutexUnlock(&gClassRegistryState.lock);
	BCMutexDestroy(&gClassRegistryState.lock);
}

// =========================================================
// MARK: Class Public
// =========================================================

BCClassId BCClassRegistryGetCount(void) {
	return gClassRegistryState.total_classes;
}

BCClassId BCClassRegister(const BCClassRef cls) {
	BCMutexLock(&gClassRegistryState.lock);

	// Check if we need to allocate a new segment
	const uint32_t current_index = gClassRegistryState.total_classes;
	const uint32_t required_segment = GetSegmentIndex(current_index);

	// Allocate segments as needed
	while (gClassRegistryState.segment_count <= required_segment) {
		if (gClassRegistryState.segment_count >= BC_CLASS_REGISTRY_MAX_SEGMENTS) {
			BCMutexUnlock(&gClassRegistryState.lock);
			return BC_CLASS_ID_INVALID; // gClassRegistryState is full
		}

		const uint32_t segment_size = GetSegmentSize(gClassRegistryState.segment_count);
		gClassRegistryState.segments[gClassRegistryState.segment_count] =
			BCMalloc(segment_size * sizeof(BCClass*));

		if (!gClassRegistryState.segments[gClassRegistryState.segment_count]) {
			BCMutexUnlock(&gClassRegistryState.lock);
			return BC_CLASS_ID_INVALID; // Allocation failed
		}

		// Zero out the new segment
		memset(gClassRegistryState.segments[gClassRegistryState.segment_count], 0,
			   segment_size * sizeof(BCClass*));

		gClassRegistryState.segment_count++;
	}

	// Store the class pointer in the registry
	const uint32_t segment = GetSegmentIndex(current_index);
	const uint32_t offset = GetSegmentOffset(current_index);

	gClassRegistryState.segments[segment][offset] = cls;
	gClassRegistryState.total_classes++;
	cls->id = current_index;
	BCMutexUnlock(&gClassRegistryState.lock);

	return current_index;
}

BCClassRef BCClassIdToRef(const BCClassId cid) {
	if (cid >= gClassRegistryState.total_classes) {
		return NULL;
	}

	const uint32_t segment = GetSegmentIndex(cid);
	const uint32_t offset = GetSegmentOffset(cid);

	if (segment >= gClassRegistryState.segment_count || !gClassRegistryState.segments[segment]) {
		return NULL;
	}

	return gClassRegistryState.segments[segment][offset];
}

BCClassId BCClassRefToId(const BCClassRef cls) {
	if (cls == NULL) { return BC_CLASS_ID_INVALID; }
	BCMutexLock(&gClassRegistryState.lock);

	// Slow Linear Search, only for debugging purposes
	for (uint32_t i = 0; i < gClassRegistryState.total_classes; i++) {
		if (BCClassIdToRef(i) == cls) {
			BCMutexUnlock(&gClassRegistryState.lock);
			return i;
		}
	}

	BCMutexUnlock(&gClassRegistryState.lock);
	return BC_CLASS_ID_INVALID;
}

// =========================================================
// MARK: Internal
// =========================================================

static inline uint32_t GetSegmentIndex(const uint32_t class_index) {
	if (class_index < BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE) { return 0; }

	// Find the highest bit set to determine which segment
	// For example: index 100 (binary: 1100100)
	// - Subtract initial size: 100 - 32 = 68
	// - Find highest bit: 68 = 0b1000100, highest bit at position 6
	// - Segment = 1 (second segment, size 64)

	const uint32_t adjusted = class_index - BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE;

	// Count leading zeros to find highest bit
	// __builtin_clz returns leading zeros for uint32_t
	const uint32_t highest_bit = 31 - __builtin_clz(adjusted | 1);

	return highest_bit + 1;
}

static inline uint32_t GetSegmentOffset(const uint32_t class_index) {
	if (class_index < BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE) {
		return class_index;
	}

	const uint32_t segment = GetSegmentIndex(class_index);

	// Calculate the start index of this segment
	// Segment 0: starts at 0 (size 32)
	// Segment 1: starts at 32 (size 64)
	// Segment 2: starts at 96 (size 128)
	// Formula: 32 + 64 + ... = 32 * (2^segment - 1)
	const uint32_t segment_start = BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE * ((1u << segment) - 1);

	return class_index - segment_start;
}

static inline uint32_t GetSegmentSize(const uint32_t segment_index) {
	return BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE << segment_index;
}