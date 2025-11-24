#ifndef BCRUNTIME_BCKEYWORDS_H
#define BCRUNTIME_BCKEYWORDS_H

#include "../BCSettings.h"

#define $VAR __auto_type
#define $LET const __auto_type
#define $TYPE(_type_) __typeof__(_type_)

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1
#define $TLS _Thread_local
#else
#define $TLS
#endif

#endif //BCRUNTIME_BCKEYWORDS_H