#ifndef BCRUNTIME_BCTHREADS_H
#define BCRUNTIME_BCTHREADS_H

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

#if defined(_WIN32)
typedef CRITICAL_SECTION BCMutex;
#else
typedef pthread_mutex_t BCMutex;
#endif

void BCMutexInit(BCMutex *mutex);
void BCMutexLock(BCMutex *mutex);
void BCMutexUnlock(BCMutex *mutex);
void BCMutexDestroy(BCMutex *mutex);

// =========================================================
// MARK: Run Once
// =========================================================

#if defined(_WIN32)
typedef INIT_ONCE BCOnceToken;
#define BC_ONCE_INIT INIT_ONCE_STATIC_INIT
#else
typedef pthread_once_t BCOnceToken;
#define BC_ONCE_INIT PTHREAD_ONCE_INIT
#endif

void BCRunOnce(BCOnceToken* token, void (*func)(void));

#ifdef __cplusplus
}
#endif

#endif //BCRUNTIME_BCTHREADS_H
