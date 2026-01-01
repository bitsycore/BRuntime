#ifndef BCORE_STRING_COMPAT_H
#define BCORE_STRING_COMPAT_H

#include <stddef.h>

#define BC_STRCAT_S_OK          0
#define BC_STRCAT_S_EINVAL      22 /* invalid argument */
#define BC_STRCAT_S_ERANGE      34 /* buffer too small */

#ifdef _MSC_VER
#define BC_strcat_s(dest, dest_sz, src) strcat_s(dest, dest_sz, src)
#else
int BC_strcat_s(char * dest,size_t dest_sz, const char * src);
#endif

#endif //BCORE_STRING_COMPAT_H