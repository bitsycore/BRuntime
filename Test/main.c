#define BC_RUNTIME_MAIN
#include <BCRuntime/BCRuntime.h>

#include "tests/tests.h"

int RETRY = 1;

void demo(void) {
	BCAutoreleaseScope() {

		$LET str = $("Boxed");
		$LET strAutorelease = $$("Boxed Autoreleased");

		$LET formatedStr = $("String with construction %d", 123);
		$LET stringInPool = BCStringPooledLiteral("String in global pool");


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

		const char* buffer = BCAllocatorAlloc(kBCAllocatorRefSystem, 4096);
		BCArenaRef arena = BCArenaCreate(NULL, buffer, 4096);
		const BCAllocatorRef allocator = BCArenaAllocator(arena);

		$LET sb = BCStringBuilderCreate(allocator);
		BCAutorelease($OBJ sb);
		BCStringBuilderAppend(sb, "Hello from SB");
		BCStringBuilderAppendChar(sb, ' ');
		BCStringBuilderAppendChar(sb, '!');
		BCStringBuilderAppendChar(sb, ' ');
		BCStringBuilderAppendString(sb, str);
		BCStringBuilderAppendChar(sb, ' ');
		BCStringBuilderAppendFormat(sb, "Number: %f", 3.141);
		$LET buildedStr = BCStringBuilderFinish(sb);

		printf("Str: %s", BCStringCPtr(buildedStr));

		BCAutoreleaseAll(
			$OBJ str,
			$OBJ formatedStr,
			$OBJ list,
			$OBJ map,
			$OBJ buildedStr
		);
	}
}

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
			testStringBuilder();
			testReleasePool();
			testClassRegistry();

			demo();

			BCObjectDebugDump();
			BCStringPoolDebugDump();
		}

		BIG_TITLE("BC End");
	}

	return 0;
}
