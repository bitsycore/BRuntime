#define BF_RUNTIME_MAIN
#include <BFramework/BF.h>

#include "BCore/BC_Macro.h"

#include "../BFramework/BF_Format.h"
#include "BFramework/Object/BO_String.h"

#include "tests/tests.h"

void demo(void) {
	BF_AutoreleaseScope() {

		$LET str = $("Boxed");
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

		BFAutoreleaseAll(
			$OBJ str,
			$OBJ formatedStr,
			$OBJ list,
			$OBJ map
		);
	}

}

int RETRY = 1;

int BCMain() {
	BO_ObjectDebugSetEnabled(BC_true);
	BO_ObjectDebugSetKeepFreed(BC_false);

	for (int i = 0; i < RETRY; i++) {

		BIG_TITLE("BC Startup");

		BF_AutoreleaseScope() {
			testNumber();
			testString();
			testArray();
			testMap();
			testStringBuilder();
			testReleasePool();
			testClassRegistry();
			testBytesArray();

			demo();

			BO_ObjectDebugDump();
			BF_StringPoolDebugDump();

		    BF_printf("Hello world %@", $$LIST("Hello", 1, 3, 4));
		}
		BIG_TITLE("BC End");
	}

	benchmarkAutoreleasePool();

	return 0;
}
