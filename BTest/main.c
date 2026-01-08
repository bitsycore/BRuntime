#define BF_IMPLEMENT_MAIN
#include <BFramework/BF.h>

#include "Benchmarks/BT_Benchmarks.h"
#include "Tests/BT_Tests.h"

void BT_Demo(void) {
	BF_AutoreleaseScope() {
		$LET str = $("Boxed %s", "String Demo");
		BF_Print("%@ \n", str);

		$LET bytes = BO_BytesArrayCreate(64);
		BF_Autorelease($OBJ bytes);
		BO_BytesArrayFill(bytes, 'A');
		$LET copiedBytes = (BO_BytesArrayRef)BO_Copy($OBJ bytes);
		BF_Autorelease($OBJ copiedBytes);
		BO_BytesArrayFill(bytes, 'D');

		BF_PrintString((char*)BO_BytesArrayBytes(bytes), 64, "Bytes Array Length: %zu\n", BO_BytesArraySize(bytes));
		BF_Print("%.*s \n", BO_BytesArraySize(bytes), BO_BytesArrayBytes(bytes));

		$LET strAutorelease = $$("Boxed Autoreleased");

		$LET formatedStr = $("String with construction %@", str);
		$LET stringInPool = BO_StringPooledLiteral("String in global pool");


		$LET list = $LIST("List Demo", 1, 2, 3, 4, 5);
		$$LIST("Autoreleased list demo");

		// $$LIST are autoreleased and $LIST push and pop an autorelease pool during it's creation allowing to free inner created objects
		// so use $$LIST when creating list directly inside a list
		$$LIST(
			"List with inner list",
			$$LIST(
				"Inner Autoreleased List",
				"A",
				"B",
				$$LIST("Inner Autoreleased List", "A", "B", "C")
			)
		);

		$LET map = $MAP("Key1", "Value1", "Key2", "Value2", "Key3", "Value3");
		$$MAP("Autoreleased map", "demo");

		$$MAP( // $$MAP work like $$LIST
			"Autoreleased map with inner map", "val",
			"InnerMap1", $$MAP("InnerKey1", "InnerValue1", "InnerKey2", "InnerValue2"),
			"InnerMap2", $$MAP("InnerKeyA", "InnerValueA", "InnerKeyB", "InnerValueB")
		);

		BF_AutoreleaseAll(
			$OBJ str,
			$OBJ formatedStr,
			$OBJ list,
			$OBJ map
		);
	}

}

int RETRY = 1;

int BF_Main() {
	BO_ObjectDebugSetEnabled(BC_true);
	BO_ObjectDebugSetKeepFreed(BC_false);

	BT_Demo();

	BO_ObjectDebugDump();
	BO_StringPoolDebugDump();
	return 0;
	for (int i = 0; i < RETRY; i++) {

		BT_PrintBigTitle("BC Startup");

		BF_AutoreleaseScope() {
			BT_TestNumber();
			BT_TestString();
			BT_TestArray();
			BT_TestMap();
			BT_TestStringBuilder();
			BT_TestReleasePool();
			BT_TestClassRegistry();
			BT_TestBytesArray();

			BT_Demo();

			BO_ObjectDebugDump();
			BO_StringPoolDebugDump();

		    BF_Print("Hello world %@", $$LIST("Hello", 1, 3, 4));
		}
		BT_PrintBigTitle("BC End");
	}

	BT_BenchmarkAutoreleasePool();

	return 0;
}
