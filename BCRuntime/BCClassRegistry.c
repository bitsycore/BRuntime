#include "BCClassRegistry.h"

#include "BCClass.h"
#include "Utilities/BCMemory.h"
#include "Utilities/BCThreads.h"

#include <stddef.h>
#include <string.h>

// =========================================================
// MARK: Configuration
// =========================================================

// Maximum number of segments (8 segments = 64 + ... + 8192 = 16,320 classes)
#define BC_CLASS_REGISTRY_MAX_SEGMENTS 8

// Initial segment size (must be power of 2)
#define BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE 64

// Bit shift for initial segment (log2(64) = 6)
#define BC_CLASS_REGISTRY_INITIAL_SHIFT 6

// =========================================================
// MARK: Registry State
// =========================================================

typedef struct BCClassRegistryState {
  BC_MUTEX_MAYBE(lock)

  // Array of segment pointers (static allocation)
  BCClass **segments[BC_CLASS_REGISTRY_MAX_SEGMENTS];

  uint32_t segment_count;

  uint32_t total_classes;

} BCClassRegistryState;

static BCClassRegistryState Registry;

// =========================================================
// MARK: Segment Math Helpers
// =========================================================

/**
 * Get the segment index for a given class index.
 * Uses bit operations for fast calculation.
 */
static inline uint32_t GetSegmentIndex(const uint32_t class_index) {
  if (class_index < BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE) {
    return 0;
  }

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

/**
 * Get the offset within a segment for a given class index.
 */
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

/**
 * Get the size of a given segment.
 */
static inline uint32_t GetSegmentSize(const uint32_t segment_index) {
  return BC_CLASS_REGISTRY_INITIAL_SEGMENT_SIZE << segment_index;
}

// =========================================================
// MARK: Public API
// =========================================================

void ___BCINTERNAL___ClassRegistryInitialize(void) {
  BCMutexInit(&Registry.lock);
  memset(Registry.segments, 0, sizeof(Registry.segments));
  Registry.segment_count = 0;
  Registry.total_classes = 0;
}

void ___BCINTERNAL___ClassRegistryDeinitialize(void) {
  BCMutexLock(&Registry.lock);

  // Free all allocated segments
  for (uint32_t i = 0; i < Registry.segment_count; i++) {
    if (Registry.segments[i]) {
      BCFree(Registry.segments[i]);
      Registry.segments[i] = NULL;
    }
  }

  Registry.segment_count = 0;
  Registry.total_classes = 0;

  BCMutexUnlock(&Registry.lock);
  BCMutexDestroy(&Registry.lock);
}

BCClassId BCClassRegister(const BCClassRef cls) {
  BCMutexLock(&Registry.lock);

  // Check if we need to allocate a new segment
  const uint32_t current_index = Registry.total_classes;
  const uint32_t required_segment = GetSegmentIndex(current_index);

  // Allocate segments as needed
  while (Registry.segment_count <= required_segment) {
    if (Registry.segment_count >= BC_CLASS_REGISTRY_MAX_SEGMENTS) {
      BCMutexUnlock(&Registry.lock);
      return UINT32_MAX; // Registry is full
    }

    const uint32_t segment_size = GetSegmentSize(Registry.segment_count);
    Registry.segments[Registry.segment_count] =
        BCMalloc(segment_size * sizeof(BCClass *));

    if (!Registry.segments[Registry.segment_count]) {
      BCMutexUnlock(&Registry.lock);
      return UINT32_MAX; // Allocation failed
    }

    // Zero out the new segment
    memset(Registry.segments[Registry.segment_count], 0,
           segment_size * sizeof(BCClass *));

    Registry.segment_count++;
  }

  // Store the class pointer in the registry
  const uint32_t segment = GetSegmentIndex(current_index);
  const uint32_t offset = GetSegmentOffset(current_index);

  Registry.segments[segment][offset] = cls;
  Registry.total_classes++;

  BCMutexUnlock(&Registry.lock);

  return current_index;
}

BCClassRef BCClassIdToRef(const BCClassId classId) {
  if (classId >= Registry.total_classes) {
    return NULL;
  }

  const uint32_t segment = GetSegmentIndex(classId);
  const uint32_t offset = GetSegmentOffset(classId);

  if (segment >= Registry.segment_count || !Registry.segments[segment]) {
    return NULL;
  }

  return Registry.segments[segment][offset];
}

uint32_t BCClassRefToId(const BCClassRef cls) {
  if (cls == NULL) {
    return UINT32_MAX;
  }

  BCMutexLock(&Registry.lock);

  // Linear search through all registered classes
  // This is slow but only needed for initialization/debugging
  for (uint32_t i = 0; i < Registry.total_classes; i++) {
    if (BCClassIdToRef(i) == cls) {
      BCMutexUnlock(&Registry.lock);
      return i;
    }
  }

  BCMutexUnlock(&Registry.lock);
  return UINT32_MAX; // Not found
}

uint32_t BCClassRegistryGetCount(void) { return Registry.total_classes; }
