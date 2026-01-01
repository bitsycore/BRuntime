add_executable(BTest
		main.c
		tests/common.c
		tests/test_array.c
		tests/test_bytes_array.c
		tests/test_class_registry.c
		tests/test_map.c
		tests/test_numbers.c
		tests/test_pool.c
		tests/test_pool_benchmark.c
		tests/test_string.c
		tests/tests.h
)

target_include_directories(
		BTest PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(BTest BCore-STATIC BFramework-STATIC)