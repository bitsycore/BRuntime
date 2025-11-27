#include "BC_Compat.h"

#include <string.h>

#ifndef _MSC_VER

int BC_strcat_s(char *dest, const size_t dest_sz, const char *src) {
	if (!dest || !src || dest_sz == 0)
		return BC_STRCAT_S_EINVAL;

	const size_t dest_len = strnlen(dest, dest_sz);
	if (dest_len == dest_sz)
		return BC_STRCAT_S_EINVAL;

	const size_t src_len = strlen(src);

	/* Need room for src_len characters + '\0' */
	if (dest_len + src_len + 1 > dest_sz) {
		dest[0] = '\0';
		return BC_STRCAT_S_ERANGE;
	}

	memcpy(dest + dest_len, src, src_len + 1);
	return BC_STRCAT_S_OK;
}

#endif