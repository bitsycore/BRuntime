#include "BT_Tests.h"

void testNumber() {
	// ================================
	SUB_TITLE("Test Number");
	// ================================
	$LET numInt8 =  BO_NumberCreate((int8_t)-64);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numInt8) == BO_NumberTypeInt8);
	$LET numUInt8 =  BO_NumberCreate((uint8_t)255);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numUInt8) == BO_NumberTypeUInt8);

	$LET numInt16 =  BO_NumberCreate((int16_t)-4500);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numInt16) == BO_NumberTypeInt16);
	$LET numUInt16 =  BO_NumberCreate((uint16_t)6500);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numUInt16) == BO_NumberTypeUInt16);

	$LET numInt32 =  BO_NumberCreate((int32_t)-15500);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numInt32) == BO_NumberTypeInt32);
	$LET numUInt32 =  BO_NumberCreate((uint32_t)135255);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numUInt32) == BO_NumberTypeUInt32);

	$LET numInt64 =  BO_NumberCreate((int64_t)-0x7FFFFFFFFFFFFFFF);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numInt64) == BO_NumberTypeInt64);
	$LET numUInt64 =  BO_NumberCreate((uint64_t)0xFFFFFFFFFFFFFFFF);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numUInt64) == BO_NumberTypeUInt64);

	$LET numFloat =  BO_NumberCreate(3.14f);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numFloat) == BO_NumberTypeFloat);
	$LET numDouble =  BO_NumberCreate(-5.123456789);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numDouble) == BO_NumberTypeDouble);

	$LET numInt =  BO_NumberCreate(42);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numInt) == BO_NumberTypeInt32);
	$LET numBool = BCBool(BC_true);
	PRINT_ERR_IF_NOT(BO_NumberGetType(numBool) == BO_NumberTypeBool);

	BFAutoreleaseAll(
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

	log_fmt("Numbers Dump: \n");
	log_fmt("Int8: %s, Expect: -64\n", TO_STR(numInt8));
	log_fmt("UInt8: %s, Expect: 255\n", TO_STR(numUInt8));
	log_fmt("Int16: %s, Expect: -4500\n", TO_STR(numInt16));
	log_fmt("UInt16: %s, Expect: 6500\n", TO_STR(numUInt16));
	log_fmt("Int32: %s, Expect: -15500\n", TO_STR(numInt32));
	log_fmt("UInt32: %s, Expect: 135255\n", TO_STR(numUInt32));
	log_fmt("Int64: %s, Expect: -9223372036854775807\n", TO_STR(numInt64));
	log_fmt("UInt64: %s, Expect: 18446744073709551615\n", TO_STR(numUInt64));
	log_fmt("Float: %s, Expect: 3.14f\n", TO_STR(numFloat));
	log_fmt("Double: %s, Expect: -5.123456789\n", TO_STR(numDouble));
	log_fmt("Int: %s, Expect: 42\n", TO_STR(numInt));
	log_fmt("Bool: %s, Expect: true\n", TO_STR(numBool));

	int32_t valInt = 0;
	BO_NumberGetValue(numInt, &valInt);
	log_fmt("Extracted Int: %d, Expect: 42\n", valInt);

	float valFloat = 0;
	BO_NumberGetValue(numInt, &valFloat);
	log_fmt("Extracted float: %f, Expect: 42.0000\n", valFloat);

	int64_t valInt64 = 0;
	BO_NumberGetValue(numDouble, &valInt64);
	log_fmt("Extracted int64_t: %lld, Expect: -5\n", valInt64);

	double valDouble = 0;
	BO_NumberGetValue(numDouble, &valDouble);
	log_fmt("Extracted double: %lf, Expect: -5.123456\n", valDouble);

	// Equality
	const BO_NumberRef numInt2 = BO_NumberCreate(42);
	const BO_NumberRef numFloat2 = BO_NumberCreate(42.0f);
	BFAutorelease($OBJ numInt2);
	BFAutorelease($OBJ numFloat2);

	log_fmt("42 (Int) == 42 (Int): %s\n", BO_Equal($OBJ numInt, $OBJ numInt2) ? "YES" : "NO");
	log_fmt("42 (Int) == 42.0 (Float): %s\n", BO_Equal($OBJ numInt, $OBJ numFloat2) ? "YES" : "NO");
	log_fmt("42 (Int) == 3.14 (Float): %s\n", BO_Equal($OBJ numInt, $OBJ numFloat) ? "YES" : "NO");
}