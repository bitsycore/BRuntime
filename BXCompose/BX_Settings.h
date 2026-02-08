#ifndef BCOMPOSE_SETTINGS_H
#define BCOMPOSE_SETTINGS_H

// =========================================================
// MARK: BCompose Configuration
// =========================================================

// Default arena size for frame allocations (64KB)
#define BX_SETTINGS_FRAME_ARENA_SIZE (64 * 1024)

// Maximum state slots per node
#define BX_SETTINGS_MAX_STATE_SLOTS 32

// Default node pool capacity
#define BX_SETTINGS_NODE_POOL_CAPACITY 256

// Enable debug logging
#ifndef BX_SETTINGS_DEBUG
#define BX_SETTINGS_DEBUG 1
#endif

#endif //BCOMPOSE_SETTINGS_H
