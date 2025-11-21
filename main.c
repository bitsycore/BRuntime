#include "BCRuntime/BCObject.h"
#include "BCRuntime/BCAutoreleasePool.h"
#include "BCRuntime/BCString.h"
#include "BCRuntime/BCArray.h"
#include "BCRuntime/BCDictionary.h"
#include "BCRuntime/BCNumber.h"
#include "BCRuntime/BCRuntime.h"

#define BIG_TITLE(_x_) printf("\n=================================================================\n                      " _x_ "\n=================================================================\n")
#define SUB_TITLE(_x_) printf("\n-------------------------------\n        " _x_ "\n-------------------------------\n\n")

void testString() {
	// ================================
	SUB_TITLE("Test String");
	// ================================

	BCStringRef str1 = BCStringConst("username"); // Pooled
	BCStringRef str2 = BCStringConst("username"); // Same Instance
	BCStringRef str3 = BCStringCreate("username"); // New Instance
	BCAutorelease($OBJ str3);

	printf("Pool Test: s1=%p, s2=%p (SamePtr? %s)\n", str1, str2, (str1 == str2) ? "YES" : "NO");
	printf("Alloc Test: s1=%p, s3=%p (SamePtr? %s)\n", str1, str3, (str1 == str3) ? "YES" : "NO");
	printf("Equality Test (s1 vs s3): %s\n", BCEqual( $OBJ str1, $OBJ str3) ? "TRUE" : "FALSE");
}

void testArray() {
	// ================================
	SUB_TITLE("Test Array");
	// ================================

	$VAR numIntAuto = $(5);
	BCAutorelease($OBJ numIntAuto);

	$VAR a = $("Hello");
	BCAutorelease($OBJ a);

	$VAR arrayAuto = $ARR(a, 5, 6, 7, 8);
	BCAutorelease($OBJ arrayAuto);

	BCArrayRef array = BCArrayCreate();
	BCAutorelease($OBJ array);

	BCArrayAdd(array, $OBJ BCStringConst("Admin"));
	BCArrayAdd(array, $OBJ BCStringConst("Editor"));
	BCArrayAdd(array, $OBJ kBCTrue);

	printf("Array Dump: \n");
	BCDescription($OBJ array, 0);

	printf("\nGet Element: \n");
	BCDescription(BCArrayGet(array, 0), 0);
	puts("");
	BCDescription(BCArrayGet(array, 1), 0);
	puts("");
}


void testNumber() {
	// ================================
	SUB_TITLE("Test Number");
	// ================================

	$CONST numInt8 =  BCNumberCreate((uint8_t)0x12);
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
	BCDescription($OBJ numInt, 0);
	puts("");
	BCDescription($OBJ numFloat, 0);
	puts("");
	BCDescription($OBJ numDouble, 0);
	puts("");
	BCDescription($OBJ numInt64, 0);
	puts("");
	BCDescription($OBJ numUInt64, 0);
	puts("");

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

	BCStringRef str1 = BCStringConst("username");
	BCStringRef str2 = BCStringConst("username");

	$CONST numInt2 = $(42);
	BCAutorelease($OBJ numInt2);

	BCMutableDictionaryRef dictionary = BCMutableDictionaryCreate();
	BCAutorelease($OBJ dictionary);

	BCStringRef key = BCStringCreate("id");
	BCAutorelease($OBJ key);
	BCStringRef value = BCStringCreate("10%d", 1);
	BCAutorelease($OBJ value);

	// Key is "username" (Pooled), Value is Array
	BCDictionarySet(dictionary, $OBJ (str1), $OBJ (array));
	BCDictionarySet(dictionary, $OBJ (BCStringConst("test")), $OBJ (numInt2));
	BCDictionarySet(dictionary, $OBJ (key), $OBJ (value));

	// Description of Dictionary
	printf("Dictionary Dump: \n");
	puts("------------------------");
	BCDescription($OBJ dictionary, 0);

	puts("------------------------");

	// Get Value by Key
	printf("\"username\": \n");
	const BCObjectRef found = BCDictionaryGet( dictionary, $OBJ str2); // Look up using pooled string
	if (found) BCDescription(found, 0);

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

	BCDescription($OBJ autoDic, 0);

	puts("------------------------");

	$CONST dic = $DIC(
		"title", "Test Dictionary",
		"version", "1.0.0",
		"author", "Beej"
	);

	BCAutorelease($OBJ dic);
	BCDescription($OBJ dic, 0);
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