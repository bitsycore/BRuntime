#include "tests.h"

void testMap() {
	// ================================
	SUB_TITLE("Test Map");
	// ================================

	$VAR array = $LIST("username", "password");
	BCAutorelease($OBJ array);

	const BCStringRef str1 = BCStringPooledLiteral("username");
	const BCStringRef str2 = BCStringPooledLiteral("username");

	$LET numInt2 = $$("%d", 10);
	const BCMutableMapRef dictionary = BCMutableMapCreate();
	BCAutorelease($OBJ dictionary);

	const BCStringRef key = BCStringCreate("id");
	BCAutorelease($OBJ key);
	const BCStringRef value = BCStringCreate("10%d", 1);
	BCAutorelease($OBJ value);

	BCMapSet(dictionary, $OBJ str1, $OBJ array);
	BCMapSet(dictionary, $OBJ BCStringPooledLiteral("test"), $OBJ numInt2);
	BCMapSet(dictionary, $OBJ key, $OBJ value);

	// Description of Map
	log_fmt("%s\n", TO_STR(dictionary));

	// Get Value by Key
	const BCObjectRef found = BCMapGet( dictionary, $OBJ str2); // Look up using pooled string
	if (found) {
		log_fmt("\"username\": %s\n", TO_STR(found));
		BCRelease(found);
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