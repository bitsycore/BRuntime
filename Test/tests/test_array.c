#include "tests.h"

void testArray() {
	// ================================
	SUB_TITLE("Test Array");
	// ================================
	const $VAR numIntAuto = $(5);
	BCAutorelease($OBJ numIntAuto);

	$VAR a = $("Hello");
	BCAutorelease($OBJ a);

	const $VAR arrayAuto = $LIST(a, 5, 6, 7, 8);
	BCAutorelease($OBJ arrayAuto);

	const BCListRef array = BCListCreate();
	BCAutorelease($OBJ array);

	BCListAdd(array, $OBJ BCStringPooledLiteral("Admin") );
	BCListAdd(array, $OBJ BCStringPooledLiteral("Editor") );
	BCListAdd(array, $OBJ $(BC_true));

	printf("Array Dump: %s\n", TO_STR(array));
	printf("Get Element 0: %s\n", TO_STR(BCListGet(array, 0)));
	printf("Get Element 1: %s\n", TO_STR(BCListGet(array, 1)));
}
