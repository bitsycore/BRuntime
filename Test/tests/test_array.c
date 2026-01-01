#include "tests.h"

void testArray() {
	// ================================
	SUB_TITLE("Test Array");
	// ================================
	const $VAR numIntAuto = $(5);
	BFAutorelease($OBJ numIntAuto);

	$VAR a = $("Hello");
	BFAutorelease($OBJ a);

	const $VAR arrayAuto = $LIST(a, 5, 6, 7, 8);
	BFAutorelease($OBJ arrayAuto);

	const BO_ListRef array = BO_ListCreate();
	BFAutorelease($OBJ array);

	BO_ListAdd(array, $OBJ BO_StringPooledLiteral("Admin") );
	BO_ListAdd(array, $OBJ BO_StringPooledLiteral("Editor") );
	BO_ListAdd(array, $OBJ $(BC_true));

	log_fmt("Array Dump: %s\n", TO_STR(array));
	log_fmt("Get Element 0: %s\n", TO_STR(BO_ListGet(array, 0)));
	log_fmt("Get Element 1: %s\n", TO_STR(BO_ListGet(array, 1)));
}
