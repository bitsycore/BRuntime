#include "BC/BCObject.h"
#include "BC/BCAutoreleasePool.h"
#include "BC/BCString.h"
#include "BC/BCArray.h"
#include "BC/BCDictionary.h"
#include "BC/BCNumber.h"

#define BIG_TITLE(_x_) printf("\n=================================================================\n                      " _x_ "\n=================================================================\n")
#define SUB_TITLE(_x_) printf("\n-------------------------------\n        " _x_ "\n-------------------------------\n\n")

#define $$ (BCObjectRef)
#define $(_x_) _Generic(_x_)

int main() {
	BIG_TITLE("BC Startup");

	// ===========================
	// Push Autorelease Pool
	BCPoolPush();

	// ================================
	SUB_TITLE("Test String");
	// ================================

	BCStringRef str1 = BCStringConst("username"); // Pooled
	BCStringRef str2 = BCStringConst("username"); // Same Instance
	BCStringRef str3 = BCStringCreate("username"); // New Instance

	printf("Pool Test: s1=%p, s2=%p (SamePtr? %s)\n", str1, str2, (str1 == str2) ? "YES" : "NO");
	printf("Alloc Test: s1=%p, s3=%p (SamePtr? %s)\n", str1, str3, (str1 == str3) ? "YES" : "NO");
	printf("Equality Test (s1 vs s3): %s\n", BCEqual( $$ str1, $$ str3) ? "TRUE" : "FALSE");

	// ================================
	SUB_TITLE("Test Array");
	// ================================

	BCArrayRef array = BCArrayCreate();
	BCArrayAdd(array, $$ BCStringConst("Admin"));
	BCArrayAdd(array, $$ BCStringConst("Editor"));

	printf("Array Dump: \n");
	BCLog($$ array, 0);

	printf("\nGet Element: \n");
	BCLog(BCArrayGet(array, 0), 0);
	BCLog(BCArrayGet(array, 1), 0);

	// ================================
	SUB_TITLE("Test Number");
	// ================================

	BCNumberRef numInt = BCNumberCreate(42);
	BCNumberRef numFloat = BCNumberCreate(3.14f);
	BCNumberRef numDouble = BCNumberCreate(3.14159);
	BCNumberRef numInt64 = BCNumberCreate((int64_t)9223372036854775807LL);

	printf("Numbers Dump: \n");
	BCLog($$ numInt, 0);
	BCLog($$ numFloat, 0);
	BCLog($$ numDouble, 0);
	BCLog($$ numInt64, 0);

	int32_t valInt;
	BCNumberGetValue(numInt, &valInt);
	printf("Extracted Int: %d\n", valInt);

	float valFloat;
	BCNumberGetValue(numFloat, &valFloat);
	printf("Extracted Float: %f\n", valFloat);

	// Equality
	BCNumberRef numInt2 = BCNumberCreate(42);
	BCNumberRef numFloat2 = BCNumberCreate(42.0f);
	printf("42 (Int) == 42 (Int): %s\n", BCEqual($$ numInt, $$ numInt2) ? "YES" : "NO");
	printf("42 (Int) == 42.0 (Float): %s\n", BCEqual($$ numInt, $$ numFloat2) ? "YES" : "NO");
	printf("42 (Int) == 3.14 (Float): %s\n", BCEqual($$ numInt, $$ numFloat) ? "YES" : "NO");

	BCAutorelease($$ numInt);
	BCAutorelease($$ numFloat);
	BCAutorelease($$ numDouble);
	BCAutorelease($$ numInt64);
	BCAutorelease($$ numInt2);
	BCAutorelease($$ numFloat2);

	// ================================
	SUB_TITLE("Test Dictionary");
	// ================================

	BCMutableDictionaryRef dictionary = BCMutableDictionaryCreate();
	BCDictionaryRef dictionaryCst = BCDictionaryCreate();
	BCAutorelease($$ dictionary);

	// Key is "username" (Pooled), Value is Array
	BCDictionarySet(dictionary, $$ (str1), $$ (array));
	BCDictionarySet(dictionary, $$ (BCStringConst("test")), $$ (numInt2));

	// Key is dynamic string "id", Value is dynamic string "101"
	BCStringRef key = BCStringCreate("id");
	BCStringRef value = BCStringCreate("10%d", 1);
	BCDictionarySet(dictionary, $$ (key), $$ (value));

	// Release Locals reference, Dictionary will retain its own
	BCRelease($$ array);
	BCRelease($$ key);
	BCRelease($$ value);
	BCRelease($$ str3);

	BCHash($$ dictionaryCst);

	// Description of Dictionary
	printf("Dictionary Dump: \n");
	puts("----");
	BCLog($$ dictionary, 0);

	// Get Value by Key
	printf("\nGet \"username\": \n");
	puts("----");
	BCObject* found = BCDictionaryGet( dictionary, $$ str2); // Look up using pooled string
	if (found) BCLog(found, 0);

	// ===========================
	// Pop Autorelease Pool
	BCPoolPop();

	BIG_TITLE("BC End");
	return 0;
}