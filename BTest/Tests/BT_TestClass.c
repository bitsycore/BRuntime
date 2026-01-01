#include "../Tests/BT_Tests.h"

#include <BFramework/BF_Class.h>
#include <BFramework/BObject/BO_Object.h>

static BF_Class TestClass1 = {
	.name = "TestClass1",
	.dealloc = NULL,
	.hash = NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BO_Object)
};

static BF_Class TestClass2 = {
	.name = "TestClass2",
	.dealloc = NULL,
	.hash = NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BO_Object)
};

static BF_Class TestClass3 = {
	.name = "TestClass3",
	.dealloc = NULL,
	.hash = NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BO_Object)
};

void testClassRegistry() {
	TITLE("Class Registry Tests");

	// Test 1: Basic registration
	{
		TEST("Basic class registration");

		const BF_ClassId idx1 = BF_ClassRegistryInsert(&TestClass1);
		const BF_ClassId idx2 = BF_ClassRegistryInsert(&TestClass2);
		const BF_ClassId idx3 = BF_ClassRegistryInsert(&TestClass3);

		ASSERT(idx1 != BF_CLASS_ID_INVALID, "Class 1 registered successfully");
		ASSERT(idx2 != BF_CLASS_ID_INVALID, "Class 2 registered successfully");
		ASSERT(idx3 != BF_CLASS_ID_INVALID, "Class 3 registered successfully");

		ASSERT(idx1 != idx2, "Indices are unique");
		ASSERT(idx2 != idx3, "Indices are unique");
		ASSERT(idx1 != idx3, "Indices are unique");
	}

	// Test 2: Decompression
	{
		TEST("Class decompression");

		const BF_ClassId idx1 = BCDebugClassFindId(&TestClass1);
		const BF_ClassId idx2 = BCDebugClassFindId(&TestClass2);

		const BF_Class* cls1 = BF_ClassIdGetRef(idx1);
		const BF_Class* cls2 = BF_ClassIdGetRef(idx2);

		ASSERT(cls1 == &TestClass1, "Decompression returns correct class 1");
		ASSERT(cls2 == &TestClass2, "Decompression returns correct class 2");
		ASSERT(strcmp(cls1->name, "TestClass1") == 0, "Class 1 name is correct");
		ASSERT(strcmp(cls2->name, "TestClass2") == 0, "Class 2 name is correct");
	}

	// Test 3: Round-trip compression/decompression
	{
		TEST("Compression/decompression round-trip");

		const BF_ClassId idx = BCDebugClassFindId(&TestClass1);
		const BF_Class* cls = BF_ClassIdGetRef(idx);
		const BF_ClassId idx2 = BCDebugClassFindId(cls);

		ASSERT(idx == idx2, "Round-trip preserves index");
		ASSERT(cls == &TestClass1, "Round-trip preserves class pointer");
	}

	// Test 4: Test with many classes to trigger segment growth
	{
#define NUM_TEST_CLASSES 50
#define MACRO_STRINGIFY(x) #x
#define MACRO_TOSTRING(x) MACRO_STRINGIFY(x)
		TEST("Segment growth (" MACRO_TOSTRING(NUM_TEST_CLASSES) " classes)");

		static BF_Class manyClasses[NUM_TEST_CLASSES];

		// Initialize and register many classes
		for (int i = 0; i < NUM_TEST_CLASSES; i++) {
			manyClasses[i].name = "TestClassMany";
			manyClasses[i].dealloc = NULL;
			manyClasses[i].hash = NULL;
			manyClasses[i].equal = NULL;
			manyClasses[i].toString = NULL;
			manyClasses[i].copy = NULL;
			manyClasses[i].allocSize = sizeof(BO_Object);

			const BF_ClassId idx = BF_ClassRegistryInsert(&manyClasses[i]);
			ASSERT_SILENT(idx != BF_CLASS_ID_INVALID, "Class registered in segment growth");
		}

		// Verify all can be decompressed correctly
		for (int i = 0; i < NUM_TEST_CLASSES; i++) {
			const BF_ClassId idx = manyClasses[i].id;
			const BF_Class* cls = BF_ClassIdGetRef(idx);
			ASSERT_SILENT(cls == &manyClasses[i], "Decompression after growth is incorrect");
		}

		const BF_ClassId total = BF_ClassRegistryGetCount();
		ASSERT(total > NUM_TEST_CLASSES, "Registry contains many classes");
	}

	// Test 5: Integration with BO_Object
	{
		TEST("BO_Object integration");

		// Register a class if not already registered
		BF_ClassId idx = BCDebugClassFindId(&TestClass1);
		if (idx == BF_CLASS_ID_INVALID) {
			idx = BF_ClassRegistryInsert(&TestClass1);
		}

		// Create an object with this class
		const BO_ObjectRef obj = BO_ObjectAlloc(NULL, TestClass1.id);
		ASSERT(obj != NULL, "Object allocated");

		// Verify the class can be retrieved
		const BF_Class* cls = BO_ObjectClass(obj);
		ASSERT(cls == &TestClass1, "BO_ObjectClass returns correct class");
		ASSERT(strcmp(cls->name, "TestClass1") == 0, "Object has correct class name");

		// Verify compressed pointer
		ASSERT(obj->cls == idx, " Object stores compressed index");

		BO_Release(obj);
	}

	log_fmt("\n" BC_AE_BGREEN "✓ All ClassRegistry tests passed!"BC_AE_RESET"\n");

	log_fmt("\n");
}
