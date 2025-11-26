#define BC_RUNTIME_MAIN
#include <BCRuntime/BCRuntime.h>

#include "tests/tests.h"

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
			testStringBuilder();
			testReleasePool();
			//testClassRegistry();

			BCObjectDebugDump();
			BCStringPoolDebugDump();
		}

		BIG_TITLE("BC End");
	}

	return 0;
}
