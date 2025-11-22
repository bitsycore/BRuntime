#include <time.h>

#include "BCRuntime/BCAutoreleasePool.h"
#include "BCRuntime/BCNumber.h"
#include "BCRuntime/BCObject.h"
#include "BCRuntime/BCRuntime.h"
#include "BCRuntime/BCString.h"
#include "BCRuntime/Array/BCArray.h"
#include "BCRuntime/Map/BCMap.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define LIGHT_RED "\033[91m"
#define BIG_TITLE(_x_) printf(GREEN "\n=================================================================\n                      " _x_ "\n=================================================================\n" RESET)
#define SUB_TITLE(_x_) printf(YELLOW "\n-------------------------------\n        " _x_ "\n-------------------------------\n\n" RESET)

#define TO_STR(_x_) BCStringCPtr( \
	(BCStringRef) ( BCAutorelease( \
		$OBJ BCToString( $OBJ (_x_) ) \
	) ) \
)

#define FAIL_IF_NOT(_cond_) if(!(_cond_)) { printf(RED "Assert failed: " LIGHT_RED #_cond_ RED " at: line %d\n" RESET, __LINE__); }

void testString() {
	// ================================
	SUB_TITLE("Test String");
	// ================================

	const BCStringRef str1 = BCStringPooledLiteral("username"); // Pooled
	const BCStringRef str2 = BCStringPooledLiteral("username"); // Same Instance
	const BCStringRef str3 = BCStringCreate("username"); // New Instance
	BCAutorelease($OBJ str3);

	printf("StringPool Test: s1=%p, s2=%p (SamePtr? %s)\n", str1, str2, str1 == str2 ? "YES" : "NO");
	printf("StringAlloc Test: s1=%p, s3=%p (SamePtr? %s)\n", str1, str3, str1 == str3 ? "YES" : "NO");
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

	BCArrayAdd(array, $OBJ BCStringPooledLiteral("Admin") );
	BCArrayAdd(array, $OBJ BCStringPooledLiteral("Editor") );
	BCArrayAdd(array, $OBJ kBCTrue);

	printf("Array Dump: %s\n", TO_STR(array));
	printf("Get Element 0: %s\n", TO_STR(BCArrayGet(array, 0)));
	printf("Get Element 1: %s\n", TO_STR(BCArrayGet(array, 1)));
}

void testNumber() {
	// ================================
	SUB_TITLE("Test Number");
	// ================================
	$LET numInt8 =  BCNumberCreate((int8_t)-64);
	FAIL_IF_NOT(BCNumberGetType(numInt8) == BCNumberTypeInt8);
	$LET numUInt8 =  BCNumberCreate((uint8_t)255);
	FAIL_IF_NOT(BCNumberGetType(numUInt8) == BCNumberTypeUInt8);

	$LET numInt16 =  BCNumberCreate((int16_t)-4500);
	FAIL_IF_NOT(BCNumberGetType(numInt16) == BCNumberTypeInt16);
	$LET numUInt16 =  BCNumberCreate((uint16_t)255);
	FAIL_IF_NOT(BCNumberGetType(numUInt16) == BCNumberTypeUInt16);

	$LET numInt32 =  BCNumberCreate((int32_t)-15500);
	FAIL_IF_NOT(BCNumberGetType(numInt32) == BCNumberTypeInt32);
	$LET numUInt32 =  BCNumberCreate((uint32_t)135255);
	FAIL_IF_NOT(BCNumberGetType(numUInt32) == BCNumberTypeUInt32);

	$LET numInt64 =  BCNumberCreate((int64_t)-0x7FFFFFFFFFFFFFFF);
	FAIL_IF_NOT(BCNumberGetType(numInt64) == BCNumberTypeInt64);
	$LET numUInt64 =  BCNumberCreate((uint64_t)0xFFFFFFFFFFFFFFFF);
	FAIL_IF_NOT(BCNumberGetType(numUInt64) == BCNumberTypeUInt64);
	
	$LET numFloat =  BCNumberCreate(3.14f);
	FAIL_IF_NOT(BCNumberGetType(numFloat) == BCNumberTypeFloat);
	$LET numDouble =  BCNumberCreate(-5.123456789);
	FAIL_IF_NOT(BCNumberGetType(numDouble) == BCNumberTypeDouble);

	$LET numInt =  BCNumberCreate(42);
	FAIL_IF_NOT(BCNumberGetType(numInt) == BCNumberTypeInt32);
	$LET numBool = BCBool(true);
	FAIL_IF_NOT(BCNumberGetType(numBool) == BCNumberTypeBool);

	BCAutorelease($OBJ numInt8);
	BCAutorelease($OBJ numUInt8);
	BCAutorelease($OBJ numInt16);
	BCAutorelease($OBJ numUInt16);
	BCAutorelease($OBJ numInt32);
	BCAutorelease($OBJ numUInt32);
	BCAutorelease($OBJ numInt64);
	BCAutorelease($OBJ numUInt64);
	BCAutorelease($OBJ numFloat);
	BCAutorelease($OBJ numDouble);
	BCAutorelease($OBJ numInt);
	BCAutorelease($OBJ numBool);

	printf("Numbers Dump: \n");
	printf("Int8: %s\n", TO_STR(numInt8));
	printf("UInt8: %s\n", TO_STR(numUInt8));
	printf("Int16: %s\n", TO_STR(numInt16));
	printf("UInt16: %s\n", TO_STR(numUInt16));
	printf("Int32: %s\n", TO_STR(numInt32));
	printf("UInt32: %s\n", TO_STR(numUInt32));
	printf("Int64: %s\n", TO_STR(numInt64));
	printf("UInt64: %s\n", TO_STR(numUInt64));
	printf("Float: %s\n", TO_STR(numFloat));
	printf("Double: %s\n", TO_STR(numDouble));
	printf("Int: %s\n", TO_STR(numInt));
	printf("Bool: %s\n", TO_STR(numBool));

	int32_t valInt = 0;
	BCNumberGetValue(numInt, &valInt);
	printf("Extracted Int: %d\n", valInt);

	float valFloat = 0;
	BCNumberGetValue(numInt, &valFloat);
	printf("Extracted float: %f\n", valFloat);

	int64_t valInt64 = 0;
	BCNumberGetValue(numDouble, &valInt64);
	printf("Extracted int64_t: %zd\n", valInt64);

	double valDouble = 0;
	BCNumberGetValue(numDouble, &valDouble);
	printf("Extracted double: %lf\n", valDouble);

	// Equality
	const BCNumberRef numInt2 = BCNumberCreate(42);
	const BCNumberRef numFloat2 = BCNumberCreate(42.0f);
	BCAutorelease($OBJ numInt2);
	BCAutorelease($OBJ numFloat2);

	printf("42 (Int) == 42 (Int): %s\n", BCEqual($OBJ numInt, $OBJ numInt2) ? "YES" : "NO");
	printf("42 (Int) == 42.0 (Float): %s\n", BCEqual($OBJ numInt, $OBJ numFloat2) ? "YES" : "NO");
	printf("42 (Int) == 3.14 (Float): %s\n", BCEqual($OBJ numInt, $OBJ numFloat) ? "YES" : "NO");
}

void testMap() {
	// ================================
	SUB_TITLE("Test Map");
	// ================================

	$VAR array = $ARR("username", "password");
	BCAutorelease($OBJ array);

	const BCStringRef str1 = BCStringPooledLiteral("username");
	const BCStringRef str2 = BCStringPooledLiteral("username");

	$LET numInt2 = $$("%d", 10);
	const BCMutableMapRef dictionary = BCMutableMapCreate();
	BCAutorelease($OBJ dictionary);

	const BCStringRef key = BCStringCreate("id");
	BCAutorelease($OBJ key);
	const BCStringRef value = BCStringCreate("10%d", 1);
	BCAutorelease($OBJ value);

	BCMapSet(dictionary, $OBJ str1, $OBJ array);
	BCMapSet(dictionary, $OBJ BCStringPooledLiteral("test"), $OBJ numInt2);
	BCMapSet(dictionary, $OBJ key, $OBJ value);

	// Description of Map
	printf("%s\n", TO_STR(dictionary));

	puts("------------------------");

	// Get Value by Key
	const BCObjectRef found = BCMapGet( dictionary, $OBJ str2); // Look up using pooled string
	if (found) {
		printf("\"username\": %s\n", TO_STR(found));
		BCRelease(found);
	}

	puts("------------------------");

	$VAR nine = $$(9);
	$VAR three = $$(1024);

	$LET autoDic = $$MAP(
		"nine", nine,
		"three", three,
		"array", array,
		"innerDic", $$MAP("abc", nine),
		"innerArr", $$ARR(nine, three)
	);
	printf("%s\n", TO_STR(autoDic));

	puts("------------------------");

	$LET dic = $$MAP(
		"title", "Test Map",
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
		testMap();
	}
	
	BIG_TITLE("BC End");

	BCStringPoolDebugDump();
	return 0;
}