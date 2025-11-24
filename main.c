#define BC_RUNTIME_MAIN
#include "BCRuntime/BCRuntime.h"

#include "BCRuntime/BCAutoreleasePool.h"
#include "BCRuntime/BCClass.h"
#include "BCRuntime/BCNumber.h"
#include "BCRuntime/BCObject.h"
#include "BCRuntime/BCString.h"
#include "BCRuntime/List/BCList.h"
#include "BCRuntime/Map/BCMap.h"
#include "BCRuntime/Utilities/BCAnsiEscape.h"

#include <stdio.h>

void BIG_TITLE(const char* _x_) {
	printf("\n"
		BC_AE_BGREEN "╭───────────────────────────────────────────────────────────────╮" BC_AE_RESET "\n"
		BC_AE_BGREEN "│"BC_AE_BOLD"                 %-45s " BC_AE_RESET BC_AE_BGREEN "│" BC_AE_RESET"\n"
		BC_AE_BGREEN "╰───────────────────────────────────────────────────────────────╯" BC_AE_RESET "\n",
		_x_
	);
}

void SUB_TITLE(const char* _x_) {
	printf("\n"
		BC_AE_BYELLOW "╭┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄╮" BC_AE_RESET "\n"
		BC_AE_BYELLOW "┊"BC_AE_ITALIC"   %-47s " BC_AE_RESET BC_AE_BYELLOW"┊" BC_AE_RESET"\n"
		BC_AE_BYELLOW "╰┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄╯" BC_AE_RESET "\n\n",
		_x_
	);
}

#define TO_STR(_x_) BCStringCPtr( \
	(BCStringRef) ( BCAutorelease( \
		$OBJ BCToString( $OBJ (_x_) ) \
	) ) \
)

#define FAIL_IF_NOT(_cond_) if(!(_cond_)) { printf(BC_AE_RED "Assert failed: " BC_AE_BRED #_cond_ BC_AE_RED " at: line %d\n" BC_AE_RESET, __LINE__); }

void testString() {
	// ================================
	SUB_TITLE("Test String");
	// ================================

	const BCStringRef str1 = BCStringPooledLiteral("username"); // Pooled
	const BCStringRef str2 = BCStringPooledLiteral("username"); // Same Instance
	const BCStringRef str3 = BCStringCreate("username"); // New Instance
	BCAutorelease($OBJ str3);

	$LET name = BCClassName(($OBJ str1)->cls);
	printf("Class Name: %s\n", TO_STR(name));

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

	const $VAR arrayAuto = $LIST(a, 5, 6, 7, 8);
	BCAutorelease($OBJ arrayAuto);

	const BCListRef array = BCListCreate();
	BCAutorelease($OBJ array);

	BCListAdd(array, $OBJ BCStringPooledLiteral("Admin") );
	BCListAdd(array, $OBJ BCStringPooledLiteral("Editor") );
	BCListAdd(array, $OBJ $(BC_true));

	printf("Array Dump: %s\n", TO_STR(array));
	printf("Get Element 0: %s\n", TO_STR(BCListGet(array, 0)));
	printf("Get Element 1: %s\n", TO_STR(BCListGet(array, 1)));
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
	$LET numUInt16 =  BCNumberCreate((uint16_t)6500);
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
	$LET numBool = BCBool(BC_true);
	FAIL_IF_NOT(BCNumberGetType(numBool) == BCNumberTypeBool);

	BCAutoreleaseAll(
		$OBJ numInt8,
		$OBJ numUInt8,
		$OBJ numInt16,
		$OBJ numUInt16,
		$OBJ numInt32,
		$OBJ numUInt32,
		$OBJ numInt64,
		$OBJ numUInt64,
		$OBJ numFloat,
		$OBJ numDouble,
		$OBJ numInt,
		$OBJ numBool
	);

	printf("Numbers Dump: \n");
	printf("Int8: %s, Expect: -64\n", TO_STR(numInt8));
	printf("UInt8: %s, Expect: 255\n", TO_STR(numUInt8));
	printf("Int16: %s, Expect: -4500\n", TO_STR(numInt16));
	printf("UInt16: %s, Expect: 6500\n", TO_STR(numUInt16));
	printf("Int32: %s, Expect: -15500\n", TO_STR(numInt32));
	printf("UInt32: %s, Expect: 135255\n", TO_STR(numUInt32));
	printf("Int64: %s, Expect: -9223372036854775807\n", TO_STR(numInt64));
	printf("UInt64: %s, Expect: 18446744073709551615\n", TO_STR(numUInt64));
	printf("Float: %s, Expect: 3.14f\n", TO_STR(numFloat));
	printf("Double: %s, Expect: -5.123456789\n", TO_STR(numDouble));
	printf("Int: %s, Expect: 42\n", TO_STR(numInt));
	printf("Bool: %s, Expect: true\n", TO_STR(numBool));

	int32_t valInt = 0;
	BCNumberGetValue(numInt, &valInt);
	printf("Extracted Int: %d, Expect: 42\n", valInt);

	float valFloat = 0;
	BCNumberGetValue(numInt, &valFloat);
	printf("Extracted float: %f, Expect: 42.0000\n", valFloat);

	int64_t valInt64 = 0;
	BCNumberGetValue(numDouble, &valInt64);
	printf("Extracted int64_t: %zd, Expect: -5\n", valInt64);

	double valDouble = 0;
	BCNumberGetValue(numDouble, &valDouble);
	printf("Extracted double: %lf, Expect: -5.123456\n", valDouble);

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

	$VAR array = $LIST("username", "password");
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

	// Get Value by Key
	const BCObjectRef found = BCMapGet( dictionary, $OBJ str2); // Look up using pooled string
	if (found) {
		printf("\"username\": %s\n", TO_STR(found));
		BCRelease(found);
	}

	$VAR nine = $$(9);
	$VAR three = $$(1024);

	$LET autoDic = $$MAP(
		"nine", nine,
		"three", three,
		"array", array,
		"innerDic", $$MAP("abc", nine),
		"innerArr", $$LIST(nine, three)
	);
	printf("%s\n", TO_STR(autoDic));

	$LET dic = $$MAP(
		"title", "Test Map",
		"version", "1.0.0",
		"author", "Beej",
		"isActive", BC_true
	);
	printf("%s\n", TO_STR(dic));
}

int RETRY = 1;

int BCMain() {

	BCObjectDebugSetEnabled(BC_true);
	BCObjectDebugSetKeepFreed(BC_false);

	for (int i = 0; i < RETRY; i++) {

		BIG_TITLE("BC Startup");

		BCAutoreleaseScope() {
			testNumber();
			testString();
			testArray();
			testMap();

			BCStringPoolDebugDump();
			BCObjectDebugDump();
		}

		BIG_TITLE("BC End");
	}

	return 0;
}
