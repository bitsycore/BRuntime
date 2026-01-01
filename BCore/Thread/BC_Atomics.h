#ifndef BCORE_ATOMICS_H
#define BCORE_ATOMICS_H

#include <stddef.h>
#include <stdint.h>

#include "../BC_Settings.h"

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1

#include <stdatomic.h>

#include "../BC_Types.h"

typedef _Atomic(BC_bool) BC_atomic_bool;
typedef _Atomic(uint8_t) BC_atomic_uint8;
typedef _Atomic(uint16_t) BC_atomic_uint16;
typedef atomic_uint_fast32_t BC_atomic_uint_fast32;
typedef atomic_size_t BC_atomic_size;

#define BC_atomic_fetch_add(PTR, VAL) atomic_fetch_add(PTR, VAL)
#define BC_atomic_fetch_sub(PTR, VAL) atomic_fetch_sub(PTR, VAL)
#define BC_atomic_load(PTR) atomic_load(PTR)
#define BC_atomic_store(PTR, VAL) atomic_store(PTR, VAL)

#else

typedef BC_bool BC_atomic_bool;
typedef uint8_t BC_atomic_uint8;
typedef uint16_t BC_atomic_uint16;
typedef uint_fast32_t BC_atomic_uint_fast32;
typedef size_t BC_atomic_size;

#define BC_atomic_fetch_add(PTR, VAL) ({ int ____atomic_old = *(PTR); *(PTR) += (VAL); ____atomic_old; })
#define BC_atomic_fetch_sub(PTR, VAL) ({ int ____atomic_old = *(PTR); *(PTR) -= (VAL); ____atomic_old; })
#define BC_atomic_load(PTR) (*(PTR))
#define BC_atomic_store(PTR, VAL) (*(PTR) = (VAL))

#endif

#endif //BCORE_ATOMICS_H