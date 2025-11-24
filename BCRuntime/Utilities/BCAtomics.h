#ifndef BCRUNTIME_BCATOMICS_H
#define BCRUNTIME_BCATOMICS_H

#include <stddef.h>
#include <stdint.h>

#include "../BCSettings.h"

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1

#include <stdatomic.h>
typedef atomic_int BC_atomic_int;
typedef atomic_size_t BC_atomic_size;
typedef atomic_uint_fast32_t BC_atomic_uint_fast32;
#define BC_atomic_fetch_add(PTR, VAL) atomic_fetch_add(PTR, VAL)
#define BC_atomic_fetch_sub(PTR, VAL) atomic_fetch_sub(PTR, VAL)
#define BC_atomic_load(PTR) atomic_load(PTR)
#define BC_atomic_store(PTR, VAL) atomic_store(PTR, VAL)

#else

typedef int BC_atomic_int;
typedef size_t BC_atomic_size;
typedef uint_fast32_t BC_atomic_uint_fast32;
#define BC_atomic_fetch_add(PTR, VAL) ({ int ____atomic_old = *(PTR); *(PTR) += (VAL); ____atomic_old; })
#define BC_atomic_fetch_sub(PTR, VAL) ({ int ____atomic_old = *(PTR); *(PTR) -= (VAL); ____atomic_old; })
#define BC_atomic_load(PTR) (*(PTR))
#define BC_atomic_store(PTR, VAL) (*(PTR) = (VAL))

#endif

#endif //BCRUNTIME_BCATOMICS_H