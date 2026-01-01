set(BCORE_SOURCES
		BC.c
		BC.h
		BC_Keywords.h
		BC_Macro.h
		BC_Settings.h
		BC_Types.h
		BC_range.h
		Console/BC_AnsiEscape.h
		Console/BC_LazyTable.c
		Console/BC_LazyTable.h
		Memory/BC_Allocator.c
		Memory/BC_Allocator.h
		Memory/BC_Arena.c
		Memory/BC_Arena.h
		Memory/BC_Memory.c
		Memory/BC_Memory.h
		Strings/BC_StringBuilder.c
		Strings/BC_StringBuilder.h
		Strings/BC_StringCompat.c
		Strings/BC_StringCompat.h
		Thread/BC_Atomics.h
		Thread/BC_Threads.c
		Thread/BC_Threads.h
)

add_library(BCore STATIC ${BCORE_SOURCES})