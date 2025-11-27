#ifndef BCRUNTIME_BCTHREADS_H
#define BCRUNTIME_BCTHREADS_H

#include "../BCSettings.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

// =========================================================
// MARK: Locks
// =========================================================

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1

#if defined(_WIN32)

typedef CRITICAL_SECTION BCMutex;
typedef struct { volatile LONG v; } BCSpinlock;

#else

typedef pthread_mutex_t BCMutex;
typedef pthread_spinlock_t BCSpinlock;

#endif

void BCMutexInit(BCMutex* mutex);
void BCMutexLock(BCMutex* mutex);
void BCMutexUnlock(BCMutex* mutex);
void BCMutexDestroy(BCMutex* mutex);
#define BC_MUTEX_MAYBE(_lock_name_) BCMutex _lock_name_;
#define BC_MUTEX_MAYBE_STATIC(_lock_name_) static BC_MUTEX_MAYBE(_lock_name_);

void BCSpinlockInit(BCSpinlock* sl);
void BCSpinlockLock(BCSpinlock* sl);
void BCSpinlockUnlock(BCSpinlock* sl);
void BCSpinlockDestroy(BCSpinlock* sl);
#define BC_SPINLOCK_MAYBE(_lock_name_) BCSpinlock _lock_name_;
#define BC_SPINLOCK_MAYBE_STATIC(_lock_name_) static BC_SPINLOCK_MAYBE(_lock_name_);

#else

#define BCMutexInit(_)
#define BCMutexLock(_)
#define BCMutexUnlock(_)
#define BCMutexDestroy(_)
#define BC_MUTEX_MAYBE(_)
#define BC_MUTEX_MAYBE_STATIC(_)

#define BCSpinlockInit(_)
#define BCSpinlockLock(_)
#define BCSpinlockUnlock(_)
#define BCSpinlockDestroy(_)
#define BC_SPINLOCK_MAYBE(_)
#define BC_SPINLOCK_MAYBE_STATIC(_)

#endif

// =========================================================
// MARK: Run Once
// =========================================================

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1

#if defined(_WIN32)
typedef INIT_ONCE BCOnceToken;
#define BC_ONCE_INIT INIT_ONCE_STATIC_INIT
#else
typedef pthread_once_t BCOnceToken;
#define BC_ONCE_INIT PTHREAD_ONCE_INIT
#endif

#define BC_ONCE_MAYBE_STATIC(_name_) static BCOnceToken _name_ = BC_ONCE_INIT;
void BCRunOnce(BCOnceToken* token, void (*func)(void));

#else
#define BC_ONCE_MAYBE_STATIC(_name_)
#define BCRunOnce(_, _fun_) (_fun_())
#endif

#ifdef __cplusplus
}
#endif

#endif //BCRUNTIME_BCTHREADS_H
