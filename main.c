#include "BC/BCObject.h"
#include "BC/BCAutoreleasePool.h"
#include "BC/BCString.h"
#include "BC/BCArray.h"
#include "BC/BCDictionary.h"

#define BIG_TITLE(_x_) printf("\n=================================================================\n                      " _x_ "\n=================================================================\n")
#define SUB_TITLE(_x_) printf("\n-------------------------------\n        " _x_ "\n-------------------------------\n\n")

#define CAST (void*)

int main() {
	BIG_TITLE("BC Startup");

	// ===========================
	// Push Autorelease Pool
	BCPoolPush();

	// ================================
	SUB_TITLE("Test String");
	// ================================

	BCString* str1 = BCStringConst("username"); // Pooled
	BCString* str2 = BCStringConst("username"); // Same Instance
	BCString* str3 = BCStringCreate("username"); // New Instance

	printf("Pool Test: s1=%p, s2=%p (SamePtr? %s)\n", str1, str2, (str1 == str2) ? "YES" : "NO");
	printf("Alloc Test: s1=%p, s3=%p (SamePtr? %s)\n", str1, str3, (str1 == str3) ? "YES" : "NO");
	printf("Equality Test (s1 vs s3): %s\n", BCEqual(CAST str1, CAST str3) ? "TRUE" : "FALSE");

	// ================================
	SUB_TITLE("Test Array");
	// ================================

	BCArray* array = BCArrayCreate();
	BCArrayAdd(array, CAST BCStringConst("Admin"));
	BCArrayAdd(array, CAST BCStringConst("Editor"));

	printf("Array Dump: \n");
	BCLog(CAST array);

	printf("\nGet Element: \n");
	BCLog(BCArrayGet(array, 0));
	BCLog(BCArrayGet(array, 1));


	// ================================
	SUB_TITLE("Test Dictionary");
	// ================================

	BCMutableDictionary * dictionary = BCMutableDictionaryCreate();
	BCAutorelease(CAST dictionary);

	// Key is "username" (Pooled), Value is Array
	BCDictionarySet(dictionary, CAST (str1), CAST (array));

	// Key is dynamic string "id", Value is dynamic string "101"
	BCString* key = BCStringCreate("id");
	BCString* value = BCStringCreate("10%d", 1);
	BCDictionarySet(dictionary, CAST (key), CAST (value));

	// Release Locals reference, Dictionary will retain its own
	BCRelease(CAST array);
	BCRelease(CAST key);
	BCRelease(CAST value);
	BCRelease(CAST str3);

	// Description of Dictionary
	printf("Dictionary Dump: \n");
	puts("----");
	BCLog(CAST dictionary);

	// Get Value by Key
	printf("\nGet \"username\": \n");
	puts("----");
	BCObject* found = BCDictionaryGet(CAST dictionary, CAST (str2)); // Look up using pooled string
	if (found) BCLog(found);

	// ===========================
	// Pop Autorelease Pool
	BCPoolPop();

	BIG_TITLE("BC End");
	return 0;
}