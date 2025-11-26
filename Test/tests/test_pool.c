#include "tests.h"

void testReleasePool(void) {
	$LET pool = BCReleasePoolCreate(NULL, 16);
	BCAutorelease($OBJ pool);

	$LET a = $("Hello in Release Pool");
	$LET b = $(53);
	$LET c = $(BC_true);

	BCReleasePoolAdd(pool, $OBJ a);
	BCReleasePoolAdd(pool, $OBJ b);
	BCReleasePoolAdd(pool, $OBJ c);

	log_fmt("Release Pool Finished\n");
}