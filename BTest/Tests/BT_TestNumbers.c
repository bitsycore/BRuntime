#include "BT_Tests.h"

void BT_TestNumber() {
	// ================================
	BT_PrintSubTitle("Test Number");
	// ================================
	$LET numInt8 =  BO_NumberCreate((int8_t)-64);
	BT_PrintErrIfNot(BO_NumberGetType(numInt8) == BO_NumberTypeInt8);
	$LET numUInt8 =  BO_NumberCreate((uint8_t)255);
	BT_PrintErrIfNot(BO_NumberGetType(numUInt8) == BO_NumberTypeUInt8);

	$LET numInt16 =  BO_NumberCreate((int16_t)-4500);
	BT_PrintErrIfNot(BO_NumberGetType(numInt16) == BO_NumberTypeInt16);
	$LET numUInt16 =  BO_NumberCreate((uint16_t)6500);
	BT_PrintErrIfNot(BO_NumberGetType(numUInt16) == BO_NumberTypeUInt16);

	$LET numInt32 =  BO_NumberCreate((int32_t)-15500);
	BT_PrintErrIfNot(BO_NumberGetType(numInt32) == BO_NumberTypeInt32);
	$LET numUInt32 =  BO_NumberCreate((uint32_t)135255);
	BT_PrintErrIfNot(BO_NumberGetType(numUInt32) == BO_NumberTypeUInt32);

	$LET numInt64 =  BO_NumberCreate((int64_t)-0x7FFFFFFFFFFFFFFF);
	BT_PrintErrIfNot(BO_NumberGetType(numInt64) == BO_NumberTypeInt64);
	$LET numUInt64 =  BO_NumberCreate((uint64_t)0xFFFFFFFFFFFFFFFF);
	BT_PrintErrIfNot(BO_NumberGetType(numUInt64) == BO_NumberTypeUInt64);

	$LET numFloat =  BO_NumberCreate(3.14f);
	BT_PrintErrIfNot(BO_NumberGetType(numFloat) == BO_NumberTypeFloat);
	$LET numDouble =  BO_NumberCreate(-5.123456789);
	BT_PrintErrIfNot(BO_NumberGetType(numDouble) == BO_NumberTypeDouble);

	$LET numInt =  BO_NumberCreate(42);
	BT_PrintErrIfNot(BO_NumberGetType(numInt) == BO_NumberTypeInt32);
	$LET numBool = BCBool(BC_true);
	BT_PrintErrIfNot(BO_NumberGetType(numBool) == BO_NumberTypeBool);

	BF_AutoreleaseAll(
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

	BT_Print("Numbers Dump: \n");
	BT_Print("Int8: %s, Expect: -64\n", BT_ToStr(numInt8));
	BT_Print("UInt8: %s, Expect: 255\n", BT_ToStr(numUInt8));
	BT_Print("Int16: %s, Expect: -4500\n", BT_ToStr(numInt16));
	BT_Print("UInt16: %s, Expect: 6500\n", BT_ToStr(numUInt16));
	BT_Print("Int32: %s, Expect: -15500\n", BT_ToStr(numInt32));
	BT_Print("UInt32: %s, Expect: 135255\n", BT_ToStr(numUInt32));
	BT_Print("Int64: %s, Expect: -9223372036854775807\n", BT_ToStr(numInt64));
	BT_Print("UInt64: %s, Expect: 18446744073709551615\n", BT_ToStr(numUInt64));
	BT_Print("Float: %s, Expect: 3.14f\n", BT_ToStr(numFloat));
	BT_Print("Double: %s, Expect: -5.123456789\n", BT_ToStr(numDouble));
	BT_Print("Int: %s, Expect: 42\n", BT_ToStr(numInt));
	BT_Print("Bool: %s, Expect: true\n", BT_ToStr(numBool));

	int32_t valInt = 0;
	BO_NumberGetValue(numInt, &valInt);
	BT_Print("Extracted Int: %d, Expect: 42\n", valInt);

	float valFloat = 0;
	BO_NumberGetValue(numInt, &valFloat);
	BT_Print("Extracted float: %f, Expect: 42.0000\n", valFloat);

	int64_t valInt64 = 0;
	BO_NumberGetValue(numDouble, &valInt64);
	BT_Print("Extracted int64_t: %lld, Expect: -5\n", valInt64);

	double valDouble = 0;
	BO_NumberGetValue(numDouble, &valDouble);
	BT_Print("Extracted double: %lf, Expect: -5.123456\n", valDouble);

	// Equality
	const BO_NumberRef numInt2 = BO_NumberCreate(42);
	const BO_NumberRef numFloat2 = BO_NumberCreate(42.0f);
	BF_Autorelease($OBJ numInt2);
	BF_Autorelease($OBJ numFloat2);

	BT_Print("42 (Int) == 42 (Int): %s\n", BO_Equal($OBJ numInt, $OBJ numInt2) ? "YES" : "NO");
	BT_Print("42 (Int) == 42.0 (Float): %s\n", BO_Equal($OBJ numInt, $OBJ numFloat2) ? "YES" : "NO");
	BT_Print("42 (Int) == 3.14 (Float): %s\n", BO_Equal($OBJ numInt, $OBJ numFloat) ? "YES" : "NO");
}