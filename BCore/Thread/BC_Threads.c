#include "BC_Threads.h"

// =========================================================
// MARK: Mutex Implementation
// =========================================================

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1

void BC_MutexInit(BCMutex* mutex) {
#if defined(_WIN32)
    InitializeCriticalSection(mutex);
#else
    pthread_mutex_init(mutex, NULL);
#endif
}

void BC_MutexLock(BCMutex* mutex) {
#if defined(_WIN32)
    EnterCriticalSection(mutex);
#else
    pthread_mutex_lock(mutex);
#endif
}

void BC_MutexUnlock(BCMutex* mutex) {
#if defined(_WIN32)
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_unlock(mutex);
#endif
}

void BC_MutexDestroy(BCMutex* mutex) {
#if defined(_WIN32)
    DeleteCriticalSection(mutex);
#else
    pthread_mutex_destroy(mutex);
#endif
}

void BC_SpinlockInit(BCSpinlock* sl) {
#if defined(_WIN32)
	sl->v = 0;
#elif __APPLE__
    *sl = OS_UNFAIR_LOCK_INIT;
#else
	pthread_spin_init(sl, 0);
#endif
}

void BC_SpinlockLock(BCSpinlock* sl) {
#if defined(_WIN32)
	while (InterlockedCompareExchange(&sl->v, 1, 0) != 0) {
		YieldProcessor();
	}
#elif __APPLE__
    os_unfair_lock_lock(sl);
#else
	pthread_spin_lock(sl);
#endif
}

void BC_SpinlockUnlock(BCSpinlock* sl) {
#if defined(_WIN32)
	InterlockedExchange(&sl->v, 0);
#elif __APPLE__
    os_unfair_lock_unlock(sl);
#else
	pthread_spin_unlock(sl);
#endif
}

void BC_SpinlockDestroy(BCSpinlock* sl) {
#if defined(_WIN32)
#elif __APPLE__
#else
	pthread_spin_destroy(sl);
#endif
}

// =========================================================
// MARK: Run Once Implementation
// =========================================================

#if defined(_WIN32)
static BOOL CALLBACK PRIV_WinInitOnceCallback(PINIT_ONCE InitOnce, const PVOID Parameter, PVOID *Context) {
    void (*func)(void) = (void (*)(void))Parameter;
    if (func) {
        func();
    }
    return TRUE;
}
#endif

void BC_RunOnce(BCOnceToken* token, void (*func)(void)) {
#if defined(_WIN32)
    InitOnceExecuteOnce(token, PRIV_WinInitOnceCallback, (PVOID)func, NULL);
#else
    pthread_once(token, func);
#endif
}

#endif