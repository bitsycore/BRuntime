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

void testClassRegistry()
{
    TITLE("Class Registry Tests");

    // Test 1: Basic registration
    {
        TEST("Basic class registration");

        const uint32_t idx1 = BCClassRegister(&TestClass1);
        const uint32_t idx2 = BCClassRegister(&TestClass2);
        const uint32_t idx3 = BCClassRegister(&TestClass3);

        ASSERT(idx1 != UINT32_MAX, "Class 1 registered successfully");
        ASSERT(idx2 != UINT32_MAX, "Class 2 registered successfully");
        ASSERT(idx3 != UINT32_MAX, "Class 3 registered successfully");

        ASSERT(idx1 != idx2, "Indices are unique");
        ASSERT(idx2 != idx3, "Indices are unique");
        ASSERT(idx1 != idx3, "Indices are unique");
    }

    // Test 2: Decompression
    {
        TEST("Class decompression");

        const uint32_t idx1 = BCClassCompress(&TestClass1);
        const uint32_t idx2 = BCClassCompress(&TestClass2);

        const BCClassRef cls1 = BCClassDecompress(idx1);
        const BCClassRef cls2 = BCClassDecompress(idx2);

        ASSERT(cls1 == &TestClass1, "Decompression returns correct class 1");
        ASSERT(cls2 == &TestClass2, "Decompression returns correct class 2");
        ASSERT(strcmp(cls1->name, "TestClass1") == 0, "Class 1 name is correct");
        ASSERT(strcmp(cls2->name, "TestClass2") == 0, "Class 2 name is correct");
    }

    // Test 3: Round-trip compression/decompression
    {
        TEST("Compression/decompression round-trip");

        const uint32_t idx = BCClassCompress(&TestClass1);
        const BCClassRef cls = BCClassDecompress(idx);
        const uint32_t idx2 = BCClassCompress(cls);

        ASSERT(idx == idx2, "Round-trip preserves index");
        ASSERT(cls == &TestClass1, "Round-trip preserves class pointer");
    }

    // Test 4: Test with many classes to trigger segment growth
    {
        TEST("Segment growth (100+ classes)");

#define NUM_TEST_CLASSES 100
        static BCClass manyClasses[NUM_TEST_CLASSES];

        // Initialize and register many classes
        for (int i = 0; i < NUM_TEST_CLASSES; i++)
        {
            manyClasses[i].name = "TestClassMany";
            manyClasses[i].dealloc = NULL;
            manyClasses[i].hash = NULL;
            manyClasses[i].equal = NULL;
            manyClasses[i].toString = NULL;
            manyClasses[i].copy = NULL;
            manyClasses[i].allocSize = sizeof(BCObject);

            const uint32_t idx = BCClassRegister(&manyClasses[i]);
            ASSERT(idx != UINT32_MAX, "Class registered in segment growth");
        }

        // Verify all can be decompressed correctly
        for (int i = 0; i < NUM_TEST_CLASSES; i++)
        {
            const uint32_t idx = BCClassCompress(&manyClasses[i]);
            const BCClassRef cls = BCClassDecompress(idx);
            ASSERT(cls == &manyClasses[i], "Decompression after growth is correct");
        }

        const uint32_t total = BCClassRegistryGetCount();
        ASSERT(total > 100, "Registry contains many classes");
    }

    // Test 5: Integration with BCObject
    {
        TEST("BCObject integration");

        // Register a class if not already registered
        uint32_t idx = BCClassCompress(&TestClass1);
        if (idx == UINT32_MAX)
        {
            idx = BCClassRegister(&TestClass1);
        }

        // Create an object with this class
        const BCObjectRef obj = BCObjectAlloc(NULL, &TestClass1);
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

    printf("\n");
}
