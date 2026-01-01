#ifndef BCORE_THREADS_H
#define BCORE_THREADS_H

#include "../BC_Settings.h"

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

#elif __APPLE__
#include <os/lock.h>
typedef pthread_mutex_t BCMutex;
typedef os_unfair_lock BCSpinlock;
#else

typedef pthread_mutex_t BCMutex;
typedef pthread_spinlock_t BCSpinlock;

#endif

void BC_MutexInit(BCMutex* mutex);
void BC_MutexLock(BCMutex* mutex);
void BC_MutexUnlock(BCMutex* mutex);
void BC_MutexDestroy(BCMutex* mutex);
#define BC_MUTEX_MAYBE(_lock_name_) BCMutex _lock_name_;
#define BC_MUTEX_MAYBE_STATIC(_lock_name_) static BC_MUTEX_MAYBE(_lock_name_);

void BC_SpinlockInit(BCSpinlock* sl);
void BC_SpinlockLock(BCSpinlock* sl);
void BC_SpinlockUnlock(BCSpinlock* sl);
void BC_SpinlockDestroy(BCSpinlock* sl);
#define BC_SPINLOCK_MAYBE(_lock_name_) BCSpinlock _lock_name_;
#define BC_SPINLOCK_MAYBE_STATIC(_lock_name_) static BC_SPINLOCK_MAYBE(_lock_name_);

#else

#define BC_MutexInit(_)
#define BC_MutexLock(_)
#define BC_MutexUnlock(_)
#define BC_MutexDestroy(_)
#define BC_MUTEX_MAYBE(_)
#define BC_MUTEX_MAYBE_STATIC(_)

#define BC_SpinlockInit(_)
#define BC_SpinlockLock(_)
#define BC_SpinlockUnlock(_)
#define BC_SpinlockDestroy(_)
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
void BC_RunOnce(BCOnceToken* token, void (*func)(void));

#else
#define BC_ONCE_MAYBE_STATIC(_name_)
#define BC_RunOnce(_, _fun_) (_fun_())
#endif

#ifdef __cplusplus
}
#endif

#endif //BCORE_THREADS_H
