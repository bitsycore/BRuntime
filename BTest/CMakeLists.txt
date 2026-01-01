add_executable(BTest
		Benchmarks/BT_BenchmarkAutoreleasePool.c
		Benchmarks/BT_Benchmarks.h
		Commons/BT_Common.c
		Commons/BT_Common.h
		Tests/BT_TestArray.c
		Tests/BT_TestBytesArray.c
		Tests/BT_TestClass.c
		Tests/BT_TestMap.c
		Tests/BT_TestNumbers.c
		Tests/BT_TestReleasePool.c
		Tests/BT_TestString.c
		Tests/BT_Tests.h
		main.c
)

target_include_directories(
		BTest PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(BTest BCore BFramework)