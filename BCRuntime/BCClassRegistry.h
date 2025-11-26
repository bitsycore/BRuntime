#ifndef BCRUNTIME_BCCLASSREGISTRY_H
#define BCRUNTIME_BCCLASSREGISTRY_H

#include "BCTypes.h"
#include "BCClass.h"

#include <stdint.h>

/**
 * Cleanup and free all allocated segments.
 * Should be called during runtime deinitialization.
 */
void ___BCINTERNAL___ClassRegistryDeinitialize(void);

/**
 * Register a class in the registry.
 *
 * @param cls Pointer to the BCClass to register
 * @return Compressed 32-bit index that can be used to retrieve the class
 */
uint32_t BCClassRegister(BCClass *cls);

/**
 * Decompress a 32-bit index to get the full class pointer.
 * This is the primary operation used by BCObject to get its class.
 *
 * @param classId Compressed class index (returned from BCClassRegister)
 * @return Pointer to the BCClass, or NULL if index is invalid
 */
BCClassRef BCClassIdToRef(uint32_t classId);

/**
 * Compress a class pointer to get its 32-bit index.
 * This is a reverse lookup operation (slower than decompress).
 *
 * @param cls Pointer to a registered BCClass
 * @return Compressed index, or UINT32_MAX if not found
 */
uint32_t BCClassRefToId(BCClassRef cls);

/**
 * Get the total number of registered classes.
 * Useful for statistics and debugging.
 *
 * @return Total number of classes in the registry
 */
uint32_t BCClassRegistryGetCount(void);

#endif // BCRUNTIME_BCCLASSREGISTRY_H
