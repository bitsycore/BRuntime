#include "BT_Tests.h"

void BT_TestArray() {
	// ================================
	BT_PrintSubTitle("Test Array");
	// ================================
	const $VAR numIntAuto = $(5);
	BF_Autorelease($OBJ numIntAuto);

	$VAR a = $("Hello");
	BF_Autorelease($OBJ a);

	const $VAR arrayAuto = $LIST(a, 5, 6, 7, 8);
	BF_Autorelease($OBJ arrayAuto);

	const BO_ListRef array = BO_ListCreate();
	BF_Autorelease($OBJ array);

	BO_ListAdd(array, $OBJ BO_StringPooledLiteral("Admin") );
	BO_ListAdd(array, $OBJ BO_StringPooledLiteral("Editor") );
	BO_ListAdd(array, $OBJ $(BC_true));

	BT_Print("Array Dump: %s\n", BT_ToStr(array));
	BT_Print("Get Element 0: %s\n", BT_ToStr(BO_ListGet(array, 0)));
	BT_Print("Get Element 1: %s\n", BT_ToStr(BO_ListGet(array, 1)));
}
