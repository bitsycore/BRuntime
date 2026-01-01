#include "BT_Tests.h"

void BT_TestMap() {
	// ================================
	BT_PrintSubTitle("Test Map");
	// ================================

	$VAR array = $LIST("username", "password");
	BF_Autorelease($OBJ array);

	const BO_StringRef str1 = BO_StringPooledLiteral("username");
	const BO_StringRef str2 = BO_StringPooledLiteral("username");

	$LET numInt2 = $$("%d", 10);
	const BO_MutableMapRef dictionary = BO_MutableMapCreate();
	BF_Autorelease($OBJ dictionary);

	const BO_StringRef key = BO_StringCreate("id");
	BF_Autorelease($OBJ key);
	const BO_StringRef value = BO_StringCreate("10%d", 1);
	BF_Autorelease($OBJ value);

	BO_MapSet(dictionary, $OBJ str1, $OBJ array);
	BO_MapSet(dictionary, $OBJ BO_StringPooledLiteral("test"), $OBJ numInt2);
	BO_MapSet(dictionary, $OBJ key, $OBJ value);

	// Description of Map
	BT_Print("%s\n", BT_ToStr(dictionary));

	// Get Value by Key
	const BO_ObjectRef found = BO_MapGet( dictionary, $OBJ str2); // Look up using pooled string
	if (found) {
		BT_Print("\"username\": %s\n", BT_ToStr(found));
		BO_Release(found);
	}

	$VAR nine = $$(9);
	$VAR three = $$(1024);

	$LET autoDic = $$MAP(
		"nine", nine,
		"three", three,
		"array", array,
		"innerDic", $$MAP("abc", nine),
		"innerArr", $$LIST(nine, three)
	);
	BT_Print("%s\n", BT_ToStr(autoDic));

	$LET dic = $$MAP(
		"title", "Test Map",
		"version", "1.0.0",
		"author", "Beej",
		"isActive", BC_true
	);
	BT_Print("%s\n", BT_ToStr(dic));
}