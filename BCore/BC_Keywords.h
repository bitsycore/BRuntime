#ifndef BCORE_KEYWORDS_H
#define BCORE_KEYWORDS_H

#include "BC_Settings.h"

#define BC_TYPE(_type_) __typeof__(_type_)

#if BC_SETTINGS_ENABLE_THREAD_SAFETY == 1
#define BC_TLS _Thread_local
#else
#define BC_TLS
#endif

#endif //BCORE_KEYWORDS_H