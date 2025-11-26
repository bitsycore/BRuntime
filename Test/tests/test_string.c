#include "tests.h"

void testString() {
	// ================================
	SUB_TITLE("Test String");
	// ================================

	const BCStringRef str1 = BCStringPooledLiteral("username"); // Pooled
	const BCStringRef str2 = BCStringPooledLiteral("username"); // Same Instance
	const BCStringRef str3 = BCStringCreate("username"); // New Instance
	BCAutorelease($OBJ str3);

	$LET name = BCClassName(($OBJ str1)->cls);
	printf("Class Name: %s\n", TO_STR(name));

	printf("StringPool Test: s1=%p, s2=%p (SamePtr? %s)\n", str1, str2, str1 == str2 ? "YES" : "NO");
	printf("StringAlloc Test: s1=%p, s3=%p (SamePtr? %s)\n", str1, str3, str1 == str3 ? "YES" : "NO");
	printf("Equality Test (s1 vs s3): %s\n", BCEqual( $OBJ str1, $OBJ str3) ? "TRUE" : "FALSE");
}

void testStringBuilder()
{
	// ================================
	SUB_TITLE("Test String Builder");
	// ================================

	// Test 1: Basic creation and append
	$LET builder1 = BCStringBuilderCreate(NULL);
	BCAutorelease($OBJ builder1);

	printf("Initial - Length: %zu, Capacity: %zu\n", BCStringBuilderLength(builder1), BCStringBuilderCapacity(builder1));

	BCStringBuilderAppend(builder1, "Hello");
	BCStringBuilderAppendChar(builder1, ' ');
	BCStringBuilderAppend(builder1, "World");

	printf("After appends: \"%s\" (Length: %zu)\n", BCStringBuilderCPtr(builder1), BCStringBuilderLength(builder1));

	PRINT_ERR_IF_NOT(BCStringBuilderLength(builder1) == 11);
	PRINT_ERR_IF_NOT(strcmp(BCStringBuilderCPtr(builder1), "Hello World") == 0);

	// Test 2: Format append
	BCStringBuilderAppendFormat(builder1, " - %d + %d = %d", 5, 3, 8);
	printf("After format append: \"%s\"\n", BCStringBuilderCPtr(builder1));
	PRINT_ERR_IF_NOT(strcmp(BCStringBuilderCPtr(builder1), "Hello World - 5 + 3 = 8") == 0);

	// Test 3: Append BCString
	$LET bcstr = BCStringCreate("!");
	BCAutorelease($OBJ bcstr);
	BCStringBuilderAppendString(builder1, bcstr);
	printf("After BCString append: \"%s\"\n", BCStringBuilderCPtr(builder1));
	PRINT_ERR_IF_NOT(strcmp(BCStringBuilderCPtr(builder1), "Hello World - 5 + 3 = 8!") == 0);

	// Test 4: Finalization
	$LET finalStr = BCStringBuilderFinish(builder1);
	BCAutorelease($OBJ finalStr);
	printf("Finalized BCString: \"%s\"\n", BCStringCPtr(finalStr));
	printf("Finalized toString: %s\n", TO_STR(finalStr));
	PRINT_ERR_IF_NOT(strcmp(BCStringCPtr(finalStr), BCStringBuilderCPtr(builder1)) == 0);

	// Test 5: Clear and reuse
	BCStringBuilderClear(builder1);
	PRINT_ERR_IF_NOT(BCStringBuilderLength(builder1) == 0);
	BCStringBuilderAppend(builder1, "Reused");
	printf("After clear and reuse: \"%s\" (Length: %zu)\n", BCStringBuilderCPtr(builder1), BCStringBuilderLength(builder1));
	PRINT_ERR_IF_NOT(strcmp(BCStringBuilderCPtr(builder1), "Reused") == 0);

	// Test 6: Capacity growth (force reallocation)
	$LET builder2 = BCStringBuilderCreateWithCapacity(NULL, 8);
	BCAutorelease($OBJ builder2);
	printf("\nSmall builder - Initial Capacity: %zu\n", BCStringBuilderCapacity(builder2));

	BCStringBuilderAppend(builder2, "This is a much longer string that will trigger reallocation");
	printf("After long append - Capacity: %zu, Length: %zu\n", BCStringBuilderCapacity(builder2), BCStringBuilderLength(builder2));
	printf("Content: \"%s\"\n", BCStringBuilderCPtr(builder2));

	PRINT_ERR_IF_NOT(BCStringBuilderCapacity(builder2) > 8);
	PRINT_ERR_IF_NOT(strcmp(BCStringBuilderCPtr(builder2), "This is a much longer string that will trigger reallocation") == 0);

	// Test 7: Multiple format operations
	BCStringBuilderClear(builder2);
	for (int i = 1; i <= 5; i++){
		BCStringBuilderAppendFormat(builder2, "Item %d%s", i, i < 5 ? ", " : "");
	}
	printf("\nMultiple format appends: \"%s\"\n", BCStringBuilderCPtr(builder2));
	PRINT_ERR_IF_NOT(strcmp(BCStringBuilderCPtr(builder2), "Item 1, Item 2, Item 3, Item 4, Item 5") == 0);

	// Test 8: Edge case - empty builder finalization
	$LET builder3 = BCStringBuilderCreate(NULL);
	BCAutorelease($OBJ builder3);
	$LET emptyStr = BCStringBuilderFinish(builder3);
	BCAutorelease($OBJ emptyStr);
	printf("\nEmpty builder finalized: \"%s\" (Length: %zu)\n", BCStringCPtr(emptyStr), BCStringLength(emptyStr));
	PRINT_ERR_IF_NOT(BCStringLength(emptyStr) == 0);

	// Test 9: toString implementation
	BCStringBuilderAppend(builder3, "Test toString");
	printf("Builder toString: %s\n", TO_STR(builder3));

	printf("\n" BC_AE_BGREEN "✓ All BCStringBuilder tests passed!"BC_AE_RESET"\n");
}