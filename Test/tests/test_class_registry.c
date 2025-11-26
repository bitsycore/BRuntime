#include "../tests/tests.h"

#include <BCRuntime/BCClass.h>
#include <BCRuntime/BCClassRegistry.h>
#include <BCRuntime/BCObject.h>


static BCClass TestClass1 = {
	.name = "TestClass1",
	.dealloc = NULL,
	.hash = NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BCObject)
};

static BCClass TestClass2 = {
	.name = "TestClass2",
	.dealloc = NULL,
	.hash = NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BCObject)
};

static BCClass TestClass3 = {
	.name = "TestClass3",
	.dealloc = NULL,
	.hash = NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BCObject)
};

void testClassRegistry() {
	TITLE("Class Registry Tests");

	// Test 1: Basic registration
	{
		TEST("Basic class registration");

		const BCClassId idx1 = BCClassRegister(&TestClass1);
		const BCClassId idx2 = BCClassRegister(&TestClass2);
		const BCClassId idx3 = BCClassRegister(&TestClass3);

		ASSERT(idx1 != BC_CLASS_ID_INVALID, "Class 1 registered successfully");
		ASSERT(idx2 != BC_CLASS_ID_INVALID, "Class 2 registered successfully");
		ASSERT(idx3 != BC_CLASS_ID_INVALID, "Class 3 registered successfully");

		ASSERT(idx1 != idx2, "Indices are unique");
		ASSERT(idx2 != idx3, "Indices are unique");
		ASSERT(idx1 != idx3, "Indices are unique");
	}

	// Test 2: Decompression
	{
		TEST("Class decompression");

		const BCClassId idx1 = BCClassRefToId(&TestClass1);
		const BCClassId idx2 = BCClassRefToId(&TestClass2);

		const BCClassRef cls1 = BCClassIdToRef(idx1);
		const BCClassRef cls2 = BCClassIdToRef(idx2);

		ASSERT(cls1 == &TestClass1, "Decompression returns correct class 1");
		ASSERT(cls2 == &TestClass2, "Decompression returns correct class 2");
		ASSERT(strcmp(cls1->name, "TestClass1") == 0, "Class 1 name is correct");
		ASSERT(strcmp(cls2->name, "TestClass2") == 0, "Class 2 name is correct");
	}

	// Test 3: Round-trip compression/decompression
	{
		TEST("Compression/decompression round-trip");

		const BCClassId idx = BCClassRefToId(&TestClass1);
		const BCClassRef cls = BCClassIdToRef(idx);
		const BCClassId idx2 = BCClassRefToId(cls);

		ASSERT(idx == idx2, "Round-trip preserves index");
		ASSERT(cls == &TestClass1, "Round-trip preserves class pointer");
	}

	// Test 4: Test with many classes to trigger segment growth
	{
#define NUM_TEST_CLASSES 128
#define MACRO_STRINGIFY(x) #x
#define MACRO_TOSTRING(x) MACRO_STRINGIFY(x)
		TEST("Segment growth (" MACRO_TOSTRING(NUM_TEST_CLASSES) " classes)");

		static BCClass manyClasses[NUM_TEST_CLASSES];

		// Initialize and register many classes
		for (int i = 0; i < NUM_TEST_CLASSES; i++) {
			manyClasses[i].name = "TestClassMany";
			manyClasses[i].dealloc = NULL;
			manyClasses[i].hash = NULL;
			manyClasses[i].equal = NULL;
			manyClasses[i].toString = NULL;
			manyClasses[i].copy = NULL;
			manyClasses[i].allocSize = sizeof(BCObject);

			const BCClassId idx = BCClassRegister(&manyClasses[i]);
			ASSERT_SILENT(idx != BC_CLASS_ID_INVALID, "Class registered in segment growth");
		}

		// Verify all can be decompressed correctly
		for (int i = 0; i < NUM_TEST_CLASSES; i++) {
			const BCClassId idx = BCClassRefToId(&manyClasses[i]);
			const BCClassRef cls = BCClassIdToRef(idx);
			ASSERT_SILENT(cls == &manyClasses[i], "Decompression after growth is correct");
		}

		const BCClassId total = BCClassRegistryGetCount();
		ASSERT(total > NUM_TEST_CLASSES, "Registry contains many classes");
	}

	// Test 5: Integration with BCObject
	{
		TEST("BCObject integration");

		// Register a class if not already registered
		uint32_t idx = BCClassRefToId(&TestClass1);
		if (idx == UINT32_MAX) {
			idx = BCClassRegister(&TestClass1);
		}

		// Create an object with this class
		const BCObjectRef obj = BCObjectAlloc(NULL, TestClass1.id);
		ASSERT(obj != NULL, "Object allocated");

		// Verify the class can be retrieved
		const BCClassRef cls = BCObjectClass(obj);
		ASSERT(cls == &TestClass1, "BCObjectClass returns correct class");
		ASSERT(strcmp(cls->name, "TestClass1") == 0,
			   "Object has correct class name");

		// Verify compressed pointer
		ASSERT(obj->cls == idx, "Object stores compressed index");

		BCRelease(obj);
	}

	log_fmt("\n");
}
