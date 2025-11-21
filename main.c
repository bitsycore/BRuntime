#include "BCRuntime/BCArray.h"
#include "BCRuntime/BCAutoreleasePool.h"
#include "BCRuntime/BCDictionary.h"
#include "BCRuntime/BCNumber.h"
#include "BCRuntime/BCObject.h"
#include "BCRuntime/BCRuntime.h"
#include "BCRuntime/BCString.h"

#define BIG_TITLE(_x_) printf("\n=================================================================\n                      " _x_ "\n=================================================================\n")
#define SUB_TITLE(_x_) printf("\n-------------------------------\n        " _x_ "\n-------------------------------\n\n")

#define TO_STR(_x_) BCStringGetCString( \
	(BCStringRef) ( BCAutorelease( \
		$OBJ BCToString( $OBJ (_x_) ) \
	) ) \
)

void testString() {
	// ================================
	SUB_TITLE("Test String");
	// ================================

	const BCStringRef str1 = BCStringConst("username"); // Pooled
	const BCStringRef str2 = BCStringConst("username"); // Same Instance
	const BCStringRef str3 = BCStringCreate("username"); // New Instance
	BCAutorelease($OBJ str3);

	printf("Pool Test: s1=%p, s2=%p (SamePtr? %s)\n", str1, str2, str1 == str2 ? "YES" : "NO");
	printf("Alloc Test: s1=%p, s3=%p (SamePtr? %s)\n", str1, str3, str1 == str3 ? "YES" : "NO");
	printf("Equality Test (s1 vs s3): %s\n", BCEqual( $OBJ str1, $OBJ str3) ? "TRUE" : "FALSE");
}

void testArray() {
	// ================================
	SUB_TITLE("Test Array");
	// ================================

	const $VAR numIntAuto = $(5);
	BCAutorelease($OBJ numIntAuto);

	$VAR a = $("Hello");
	BCAutorelease($OBJ a);

	const $VAR arrayAuto = $ARR(a, 5, 6, 7, 8);
	BCAutorelease($OBJ arrayAuto);

	const BCArrayRef array = BCArrayCreate();
	BCAutorelease($OBJ array);

	BCArrayAdd(array, $OBJ BCStringConst("Admin"));
	BCArrayAdd(array, $OBJ BCStringConst("Editor"));
	BCArrayAdd(array, $OBJ kBCTrue);

	printf("Array Dump: %s\n", TO_STR(array));
	printf("Get Element 0: %s\n", TO_STR(BCArrayGet(array, 0)));
	printf("Get Element 1: %s\n", TO_STR(BCArrayGet(array, 1)));
}


void testNumber() {
	// ================================
	SUB_TITLE("Test Number");
	// ================================

	$CONST numInt =  BCNumberCreate(42);
	$CONST numBool = BCNumberCreate(true);
	$CONST numFloat = BCNumberCreate(3.14f);
	$CONST numDouble = BCNumberCreate(3.14159);
	$CONST numInt64 = BCNumberCreate(9223372036854775807);
	$CONST numUInt64 = BCNumberCreate(9223372036854775807u);

	BCAutorelease($OBJ numInt);
	BCAutorelease($OBJ numFloat);
	BCAutorelease($OBJ numDouble);
	BCAutorelease($OBJ numInt64);
	BCAutorelease($OBJ numUInt64);
	BCAutorelease($OBJ numBool);

	printf("Numbers Dump: \n");
	printf("%s\n", TO_STR(numInt));
	printf("%s\n", TO_STR(numFloat));
	printf("%s\n", TO_STR(numDouble));
	printf("%s\n", TO_STR(numInt64));
	printf("%s\n", TO_STR(numUInt64));
	printf("%s\n", TO_STR(numBool));

	int32_t valInt = 0;
	BCNumberGetValue(numInt, &valInt);
	printf("Extracted Int: %d\n", valInt);

	float valFloat = 0;
	BCNumberGetValue(numInt, &valFloat);
	printf("Extracted Float: %f\n", valFloat);

	// Equality
	const BCNumberRef numInt2 = BCNumberCreate(42);
	const BCNumberRef numFloat2 = BCNumberCreate(42.0f);
	BCAutorelease($OBJ numInt2);
	BCAutorelease($OBJ numFloat2);

	printf("42 (Int) == 42 (Int): %s\n", BCEqual($OBJ numInt, $OBJ numInt2) ? "YES" : "NO");
	printf("42 (Int) == 42.0 (Float): %s\n", BCEqual($OBJ numInt, $OBJ numFloat2) ? "YES" : "NO");
	printf("42 (Int) == 3.14 (Float): %s\n", BCEqual($OBJ numInt, $OBJ numFloat) ? "YES" : "NO");
}

void testDictionary() {
	// ================================
	SUB_TITLE("Test Dictionary");
	// ================================

	$VAR array = $ARR("username", "password");
	BCAutorelease($OBJ array);

	const BCStringRef str1 = BCStringConst("username");
	const BCStringRef str2 = BCStringConst("username");

	$CONST numInt2 = $(42);
	BCAutorelease($OBJ numInt2);

	const BCMutableDictionaryRef dictionary = BCMutableDictionaryCreate();
	BCAutorelease($OBJ dictionary);

	const BCStringRef key = BCStringCreate("id");
	BCAutorelease($OBJ key);
	const BCStringRef value = BCStringCreate("10%d", 1);
	BCAutorelease($OBJ value);

	BCDictionarySet(dictionary, $OBJ str1, $OBJ array);
	BCDictionarySet(dictionary, $OBJ BCStringConst("test"), $OBJ numInt2);
	BCDictionarySet(dictionary, $OBJ key, $OBJ value);

	// Description of Dictionary
	printf("%s\n", TO_STR(dictionary));

	puts("------------------------");

	// Get Value by Key
	const BCObjectRef found = BCDictionaryGet( dictionary, $OBJ str2); // Look up using pooled string
	if (found) printf("\"username\": %s\n", TO_STR(found));

	puts("------------------------");

	$VAR nine = $$(9);
	$VAR three = $$(1024);

	const $VAR autoDic = $$DIC(
		"nine", nine,
		"three", three,
		"array", array,
		"innerDic", $$DIC("abc", nine),
		"innerArr", $$ARR(nine, three)
	);
	printf("%s\n", TO_STR(autoDic));

	puts("------------------------");

	$CONST dic = $$DIC(
		"title", "Test Dictionary",
		"version", "1.0.0",
		"author", "Beej"
	);
	printf("%s\n", TO_STR(dic));
}

int main() {
	BIG_TITLE("BC Startup");

	BCAutoreleaseScope() {
		testString();
		testArray();
		testNumber();
		testDictionary();
	}

	BIG_TITLE("BC End");
	return 0;
}