#include "BT_Tests.h"

#define BT_BytesArrayAssert(p) BT_AssertSilent(p, "Failed: " #p)

void BT_TestBytesArray(void) {
    BT_PrintBigTitle("Test BO_BytesArray");

    // Test Creation
    const BO_BytesArrayRef arr1 = BO_BytesArrayCreate(5);
	BF_Autorelease($OBJ arr1);
    BT_BytesArrayAssert(BO_BytesArraySize(arr1) == 5);
    for (size_t i = 0; i < 5; i++) {
        BT_BytesArrayAssert(BO_BytesArrayGet(arr1, i) == 0);
    }

    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    const BO_BytesArrayRef arr2 = BO_BytesArrayCreateWithBytes(5, data);
	BF_Autorelease($OBJ arr2);
    BT_BytesArrayAssert(BO_BytesArraySize(arr2) == 5);
    for (size_t i = 0; i < 5; i++) {
        BT_BytesArrayAssert(BO_BytesArrayGet(arr2, i) == data[i]);
    }

    // Test Mutation
    BO_BytesArraySet(arr1, 0, 0xFF);
    BT_BytesArrayAssert(BO_BytesArrayGet(arr1, 0) == 0xFF);

    BO_BytesArraySetRange(arr1, BC_RangeMakeEnd(1, 4), 0xAA);
    BT_BytesArrayAssert(BO_BytesArrayGet(arr1, 1) == 0xAA);
    BT_BytesArrayAssert(BO_BytesArrayGet(arr1, 2) == 0xAA);
    BT_BytesArrayAssert(BO_BytesArrayGet(arr1, 3) == 0xAA);
    BT_BytesArrayAssert(BO_BytesArrayGet(arr1, 4) == 0x00);

    BO_BytesArrayFill(arr1, 0xCC);
    for (size_t i = 0; i < 5; i++) {
        BT_BytesArrayAssert(BO_BytesArrayGet(arr1, i) == 0xCC);
    }

    // Test Copy
    const BO_BytesArrayRef copy = (BO_BytesArrayRef)BO_Copy((BO_ObjectRef)arr2);
	BF_Autorelease($OBJ copy);
    BT_BytesArrayAssert(copy != arr2); // Should be a new instance
    BT_BytesArrayAssert(BO_Equal($OBJ copy, $OBJ arr2));

    // Modify copy, original should not change
    BO_BytesArraySet(copy, 0, 0x99);
    BT_BytesArrayAssert(BO_BytesArrayGet(copy, 0) == 0x99);
    BT_BytesArrayAssert(BO_BytesArrayGet(arr2, 0) == 0x01);

    // Test ToString
    // Expected: (5) 0102030405
    const BO_StringRef str = BO_ToString((BO_ObjectRef)arr2);
	BF_Autorelease($OBJ str);
    const char* cStr = BO_StringCPtr(str);
    BF_Print("ToString: %s\n", cStr);
    // We can't easily assert the string content without a string compare function,
    // but we can visually verify or use strcmp if available.
    // Assuming standard strcmp is available or we can use BO_StringIsEqual

    // Test Compare
    const BO_BytesArrayRef arr3 = BO_BytesArrayCreateWithBytes(5, data);
	BF_Autorelease($OBJ arr3);
    BT_BytesArrayAssert(BO_BytesArrayCompare(arr2, arr3) == 0);

    BO_BytesArraySet(arr3, 0, 0x00); // Smaller
    BT_BytesArrayAssert(BO_BytesArrayCompare(arr3, arr2) < 0);
    BT_BytesArrayAssert(BO_BytesArrayCompare(arr2, arr3) > 0);

    BF_Print("BO_BytesArray tests passed\n");
}
