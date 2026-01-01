#include "tests.h"

void testReleasePool(void) {
	$LET pool = BO_ReleasePoolCreate(NULL, 16);
	BFAutorelease($OBJ pool);

	$LET a = $("Hello in Release Pool");
	$LET b = $(53);
	$LET c = $(BC_true);

	BO_ReleasePoolAdd(pool, $OBJ a);
	BO_ReleasePoolAdd(pool, $OBJ b);
	BO_ReleasePoolAdd(pool, $OBJ c);

	log_fmt("Release Pool Finished\n");
}