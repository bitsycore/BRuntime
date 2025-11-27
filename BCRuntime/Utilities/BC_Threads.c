#include "BC_Threads.h"

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

void BCSpinlockInit(BCSpinlock* sl) {
#if defined(_WIN32)
	sl->v = 0;
#else
	pthread_spin_init(sl, 0);
#endif
}

void BCSpinlockLock(BCSpinlock* sl) {
#if defined(_WIN32)
	while (InterlockedCompareExchange(&sl->v, 1, 0) != 0) {
		YieldProcessor();
	}
#else
	pthread_spin_lock(sl);
#endif
}

void BCSpinlockUnlock(BCSpinlock* sl) {
#if defined(_WIN32)
	InterlockedExchange(&sl->v, 0);
#else
	pthread_spin_unlock(sl);
#endif
}

void BCSpinlockDestroy(BCSpinlock* sl) {
#if defined(_WIN32)
	/* nothing to destroy */
#else
	pthread_spin_destroy(sl);
#endif
}

// =========================================================
// MARK: Run Once Implementation
// =========================================================


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