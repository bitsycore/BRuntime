#include "BT_Tests.h"

void BT_TestReleasePool(void) {
	$LET pool = BO_ReleasePoolCreate(NULL, 16);
	BF_Autorelease($OBJ pool);

	$LET a = $("Hello in Release Pool");
	$LET b = $(53);
	$LET c = $(BC_true);

	BO_ReleasePoolAdd(pool, $OBJ a);
	BO_ReleasePoolAdd(pool, $OBJ b);
	BO_ReleasePoolAdd(pool, $OBJ c);

	BT_Print("Release Pool Finished\n");
}