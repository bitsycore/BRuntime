#include "BT_Tests.h"

void testString() {
	// ================================
	SUB_TITLE("Test String");
	// ================================

	const BO_StringRef str1 = BO_StringPooledLiteral("username"); // Pooled
	const BO_StringRef str2 = BO_StringPooledLiteral("username"); // Same Instance
	const BO_StringRef str3 = BO_StringCreate("username"); // New Instance
	BFAutorelease($OBJ str3);

	$LET name = BF_ClassIdName(($OBJ str1)->cls);
	log_fmt("Class Name: %s\n", TO_STR(name));

	log_fmt("StringPool Test: s1=%p, s2=%p (SamePtr? %s)\n", str1, str2, str1 == str2 ? "YES" : "NO");
	log_fmt("StringAlloc Test: s1=%p, s3=%p (SamePtr? %s)\n", str1, str3, str1 == str3 ? "YES" : "NO");
	log_fmt("Equality Test (s1 vs s3): %s\n", BO_Equal( $OBJ str1, $OBJ str3) ? "TRUE" : "FALSE");
}

void testStringBuilder()
{
	// ================================
	SUB_TITLE("Test String Builder");
	// ================================

	// Test 1: Basic creation and append
	$LET builder1 = BO_StringBuilderCreate(NULL);
	BFAutorelease($OBJ builder1);

	log_fmt("Initial - Length: %zu, Capacity: %zu\n", BO_StringBuilderLength(builder1), BO_StringBuilderCapacity(builder1));

	BO_StringBuilderAppend(builder1, "Hello");
	BO_StringBuilderAppendChar(builder1, ' ');
	BO_StringBuilderAppend(builder1, "World");

	log_fmt("After appends: \"%s\" (Length: %zu)\n", BO_StringBuilderCPtr(builder1), BO_StringBuilderLength(builder1));

	PRINT_ERR_IF_NOT(BO_StringBuilderLength(builder1) == 11);
	PRINT_ERR_IF_NOT(strcmp(BO_StringBuilderCPtr(builder1), "Hello World") == 0);

	// Test 2: Format append
	BO_StringBuilderAppendFormat(builder1, " - %d + %d = %d", 5, 3, 8);
	log_fmt("After format append: \"%s\"\n", BO_StringBuilderCPtr(builder1));
	PRINT_ERR_IF_NOT(strcmp(BO_StringBuilderCPtr(builder1), "Hello World - 5 + 3 = 8") == 0);

	// Test 3: Append BO_String
	$LET bcstr = BO_StringCreate("!");
	BFAutorelease($OBJ bcstr);
	BO_StringBuilderAppendString(builder1, bcstr);
	log_fmt("After BO_String append: \"%s\"\n", BO_StringBuilderCPtr(builder1));
	PRINT_ERR_IF_NOT(strcmp(BO_StringBuilderCPtr(builder1), "Hello World - 5 + 3 = 8!") == 0);

	// Test 4: Finalization
	$LET finalStr = BO_StringBuilderFinish(builder1, BC_false);
	BFAutorelease($OBJ finalStr);
	log_fmt("Finalized BO_String: \"%s\"\n", BO_StringCPtr(finalStr));
	log_fmt("Finalized toString: %s\n", TO_STR(finalStr));
	PRINT_ERR_IF_NOT(strcmp(BO_StringCPtr(finalStr), BO_StringBuilderCPtr(builder1)) == 0);

	// Test 5: Clear and reuse
	BO_StringBuilderClear(builder1);
	PRINT_ERR_IF_NOT(BO_StringBuilderLength(builder1) == 0);
	BO_StringBuilderAppend(builder1, "Reused");
	log_fmt("After clear and reuse: \"%s\" (Length: %zu)\n", BO_StringBuilderCPtr(builder1), BO_StringBuilderLength(builder1));
	PRINT_ERR_IF_NOT(strcmp(BO_StringBuilderCPtr(builder1), "Reused") == 0);

	// Test 6: Capacity growth (force reallocation)
	$LET builder2 = BO_StringBuilderCreateWithCapacity(NULL, 8);
	BFAutorelease($OBJ builder2);
	log_fmt("\nSmall builder - Initial Capacity: %zu\n", BO_StringBuilderCapacity(builder2));

	BO_StringBuilderAppend(builder2, "This is a much longer string that will trigger reallocation");
	log_fmt("After long append - Capacity: %zu, Length: %zu\n", BO_StringBuilderCapacity(builder2), BO_StringBuilderLength(builder2));
	log_fmt("Content: \"%s\"\n", BO_StringBuilderCPtr(builder2));

	PRINT_ERR_IF_NOT(BO_StringBuilderCapacity(builder2) > 8);
	PRINT_ERR_IF_NOT(strcmp(BO_StringBuilderCPtr(builder2), "This is a much longer string that will trigger reallocation") == 0);

	// Test 7: Multiple format operations
	BO_StringBuilderClear(builder2);
	for (int i = 1; i <= 5; i++){
		BO_StringBuilderAppendFormat(builder2, "Item %d%s", i, i < 5 ? ", " : "");
	}
	log_fmt("\nMultiple format appends: \"%s\"\n", BO_StringBuilderCPtr(builder2));
	PRINT_ERR_IF_NOT(strcmp(BO_StringBuilderCPtr(builder2), "Item 1, Item 2, Item 3, Item 4, Item 5") == 0);

	// Test 8: Edge case - empty builder finalization
	$LET builder3 = BO_StringBuilderCreate(NULL);
	BFAutorelease($OBJ builder3);
	$LET emptyStr = BO_StringBuilderFinish(builder3, BC_false);
	BFAutorelease($OBJ emptyStr);
	log_fmt("\nEmpty builder finalized: \"%s\" (Length: %zu)\n", BO_StringCPtr(emptyStr), BO_StringLength(emptyStr));
	PRINT_ERR_IF_NOT(BO_StringLength(emptyStr) == 0);

	// Test 9: toString implementation
	BO_StringBuilderAppend(builder3, "Test toString");
	log_fmt("Builder toString: %s\n", TO_STR(builder3));

	log_fmt("\n" BC_AE_BGREEN "✓ All BO_StringBuilder tests passed!"BC_AE_RESET"\n");
}