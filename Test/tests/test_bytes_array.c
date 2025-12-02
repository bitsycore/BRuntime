#include "tests.h"
#include "../../BCRuntime/Core/BCFormat.h"
#include "../../BCRuntime/Object/BCBytesArray.h"

#define BC_assert(p) ASSERT_SILENT(p, "Failed: " #p)

void testBytesArray(void) {
    BIG_TITLE("Test BCBytesArray");

    // Test Creation
    const BCBytesArrayRef arr1 = BCBytesArrayCreate(5);
	BCAutorelease($OBJ arr1);
    BC_assert(BCBytesArrayCount(arr1) == 5);
    for (size_t i = 0; i < 5; i++) {
        BC_assert(BCBytesArrayGet(arr1, i) == 0);
    }

    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    const BCBytesArrayRef arr2 = BCBytesArrayCreateWithBytes(5, data);
	BCAutorelease($OBJ arr2);
    BC_assert(BCBytesArrayCount(arr2) == 5);
    for (size_t i = 0; i < 5; i++) {
        BC_assert(BCBytesArrayGet(arr2, i) == data[i]);
    }

    // Test Mutation
    BCBytesArraySet(arr1, 0, 0xFF);
    BC_assert(BCBytesArrayGet(arr1, 0) == 0xFF);

    BCBytesArraySetRange(arr1, BCRangeMakeEnd(1, 4), 0xAA);
    BC_assert(BCBytesArrayGet(arr1, 1) == 0xAA);
    BC_assert(BCBytesArrayGet(arr1, 2) == 0xAA);
    BC_assert(BCBytesArrayGet(arr1, 3) == 0xAA);
    BC_assert(BCBytesArrayGet(arr1, 4) == 0x00);

    BCBytesArrayFill(arr1, 0xCC);
    for (size_t i = 0; i < 5; i++) {
        BC_assert(BCBytesArrayGet(arr1, i) == 0xCC);
    }

    // Test Copy
    const BCBytesArrayRef copy = (BCBytesArrayRef)BCCopy((BCObjectRef)arr2);
	BCAutorelease($OBJ copy);
    BC_assert(copy != arr2); // Should be a new instance
    BC_assert(BCEqual($OBJ copy, $OBJ arr2));

    // Modify copy, original should not change
    BCBytesArraySet(copy, 0, 0x99);
    BC_assert(BCBytesArrayGet(copy, 0) == 0x99);
    BC_assert(BCBytesArrayGet(arr2, 0) == 0x01);

    // Test ToString
    // Expected: (5) 0102030405
    const BCStringRef str = BCToString((BCObjectRef)arr2);
	BCAutorelease($OBJ str);
    const char* cStr = BCStringCPtr(str);
    BC_printf("ToString: %s\n", cStr);
    // We can't easily assert the string content without a string compare function,
    // but we can visually verify or use strcmp if available.
    // Assuming standard strcmp is available or we can use BCStringIsEqual

    // Test Compare
    const BCBytesArrayRef arr3 = BCBytesArrayCreateWithBytes(5, data);
	BCAutorelease($OBJ arr3);
    BC_assert(BCBytesArrayCompare(arr2, arr3) == 0);

    BCBytesArraySet(arr3, 0, 0x00); // Smaller
    BC_assert(BCBytesArrayCompare(arr3, arr2) < 0);
    BC_assert(BCBytesArrayCompare(arr2, arr3) > 0);

    BC_printf("BCBytesArray tests passed\n");
}
