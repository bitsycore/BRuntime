#include "tests.h"

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
	printf("Extracted int64_t: %lld, Expect: -5\n", valInt64);

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