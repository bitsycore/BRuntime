cmake_minimum_required(VERSION 3.28)
project(BCRuntime C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(BlockRuntime)

add_executable(
		BCRuntime
		main.c
		BCRuntime/BCObject.c
		BCRuntime/BCObject.h
		BCRuntime/BCTypes.h
		BCRuntime/BCString.c
		BCRuntime/BCString.h
		BCRuntime/BCDictionary.c
		BCRuntime/BCDictionary.h
		BCRuntime/BCAutoreleasePool.c
		BCRuntime/BCAutoreleasePool.h
		BCRuntime/BCArray.c
		BCRuntime/BCArray.h
		BCRuntime/BCNumber.c
		BCRuntime/BCNumber.h
		BCRuntime/BCRuntime.h
		BCRuntime/BCRuntime.c
		BCRuntime/BCMacroTools.h
)

target_compile_options(BCRuntime PRIVATE -fblocks)
target_link_libraries(BCRuntime PRIVATE BlockRuntime)
add_custom_command(TARGET BCRuntime POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:BlockRuntime>
		$<TARGET_FILE_DIR:BCRuntime>
)