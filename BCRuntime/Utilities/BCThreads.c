#include "BCThreads.h"

// =========================================================
// MARK: Mutex Implementation
// =========================================================

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1

void BCMutexInit(BCMutex* mutex) {
#if defined(_WIN32)
    InitializeCriticalSection(mutex);
#else
    pthread_mutex_init(mutex, NULL);
#endif
}

void BCMutexLock(BCMutex* mutex) {
#if defined(_WIN32)
    EnterCriticalSection(mutex);
#else
    pthread_mutex_lock(mutex);
#endif
}

void BCMutexUnlock(BCMutex* mutex) {
#if defined(_WIN32)
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_unlock(mutex);
#endif
}

void BCMutexDestroy(BCMutex* mutex) {
#if defined(_WIN32)
    DeleteCriticalSection(mutex);
#else
    pthread_mutex_destroy(mutex);
#endif
}

#endif

// =========================================================
// MARK: Run Once Implementation
// =========================================================

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1

#if defined(_WIN32)
static BOOL CALLBACK WinInitOnceCallback(PINIT_ONCE InitOnce, const PVOID Parameter, PVOID *Context) {
    void (*func)(void) = (void (*)(void))Parameter;
    if (func) {
        func();
    }
    return TRUE;
}
#endif

void BCRunOnce(BCOnceToken* token, void (*func)(void)) {
#if defined(_WIN32)
    InitOnceExecuteOnce(token, WinInitOnceCallback, (PVOID)func, NULL);
#else
    pthread_once(token, func);
#endif
}

#endif