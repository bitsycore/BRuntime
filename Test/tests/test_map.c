#include "tests.h"

void testMap() {
	// ================================
	SUB_TITLE("Test Map");
	// ================================

	$VAR array = $LIST("username", "password");
	BFAutorelease($OBJ array);

	const BO_StringRef str1 = BO_StringPooledLiteral("username");
	const BO_StringRef str2 = BO_StringPooledLiteral("username");

	$LET numInt2 = $$("%d", 10);
	const BO_MutableMapRef dictionary = BCMutableMapCreate();
	BFAutorelease($OBJ dictionary);

	const BO_StringRef key = BO_StringCreate("id");
	BFAutorelease($OBJ key);
	const BO_StringRef value = BO_StringCreate("10%d", 1);
	BFAutorelease($OBJ value);

	BO_MapSet(dictionary, $OBJ str1, $OBJ array);
	BO_MapSet(dictionary, $OBJ BO_StringPooledLiteral("test"), $OBJ numInt2);
	BO_MapSet(dictionary, $OBJ key, $OBJ value);

	// Description of Map
	log_fmt("%s\n", TO_STR(dictionary));

	// Get Value by Key
	const BO_ObjectRef found = BO_MapGet( dictionary, $OBJ str2); // Look up using pooled string
	if (found) {
		log_fmt("\"username\": %s\n", TO_STR(found));
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
	log_fmt("%s\n", TO_STR(autoDic));

	$LET dic = $$MAP(
		"title", "Test Map",
		"version", "1.0.0",
		"author", "Beej",
		"isActive", BC_true
	);
	log_fmt("%s\n", TO_STR(dic));
}