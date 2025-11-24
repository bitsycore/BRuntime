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
// MARK: Mutex
// =========================================================

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1
#define BC_THREAD_MAYBE_STATIC static
#if defined(_WIN32)
typedef CRITICAL_SECTION BCMutex;
#else
typedef pthread_mutex_t BCMutex;
#endif
void BCMutexInit(BCMutex *mutex);
void BCMutexLock(BCMutex *mutex);
void BCMutexUnlock(BCMutex *mutex);
void BCMutexDestroy(BCMutex *mutex);
#define BC_MUTEX_MAYBE(_lock_name_) BCMutex _lock_name_;
#define BC_MUTEX_MAYBE_STATIC(_lock_name_) static BC_MUTEX_MAYBE(_lock_name_);
#else
#define MAYBE_STATIC
#define BCMutexInit(_)
#define BCMutexLock(_)
#define BCMutexUnlock(_)
#define BCMutexDestroy(_)
#define BC_MUTEX_MAYBE(_)
#define BC_MUTEX_MAYBE_STATIC(_)
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
