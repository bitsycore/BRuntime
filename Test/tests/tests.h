#ifndef BCRUNTIME_TESTS_H
#define BCRUNTIME_TESTS_H

#include <BCRuntime/BCClass.h>
#include <BCRuntime/BCNumber.h>
#include <BCRuntime/BCRuntime.h>
#include <BCRuntime/List/BCList.h>
#include <BCRuntime/Map/BCMap.h>
#include <BCRuntime/Pool/BCAutoreleasePool.h>
#include <BCRuntime/String/BCString.h>
#include <BCRuntime/String/BCStringBuilder.h>
#include <BCRuntime/Utilities/BCAnsiEscape.h>

#include <stdio.h>
#include <string.h>

#define TO_STR(_x_) BCStringCPtr( \
	(BCStringRef) ( BCAutorelease( \
		$OBJ BCToString( $OBJ (_x_) ) \
	) ) \
)

#define FAIL_IF_NOT(_cond_) if(!(_cond_)) { printf(BC_AE_RED "Assert failed: " BC_AE_BRED #_cond_ BC_AE_RED " at: line %d\n" BC_AE_RESET, __LINE__); }

void BIG_TITLE(const char* _x_);
void SUB_TITLE(const char* _x_);

void testArray();
void testString();
void testStringBuilder();
void testNumber();
void testMap();
void testReleasePool();

#endif //BCRUNTIME_TESTS_H