set(BFRAMEWORK_SOURCES
		BF.c
		BF.h
		BF_AutoreleasePool.c
		BF_AutoreleasePool.h
		BF_Boxing.h
		BF_Class.h
		BF_Class.c
		BF_Format.c
		BF_Format.h
		BF_Settings.h
		BF_Types.h
		BObject/BO.h
		BObject/BO_BytesArray.c
		BObject/BO_BytesArray.h
		BObject/BO_List.c
		BObject/BO_List.h
		BObject/BO_Map.c
		BObject/BO_Map.h
		BObject/BO_Number.c
		BObject/BO_Number.h
		BObject/BO_Object.c
		BObject/BO_Object.h
		BObject/BO_ReleasePool.c
		BObject/BO_ReleasePool.h
		BObject/BO_Set.c
		BObject/BO_Set.h
		BObject/BO_String.c
		BObject/BO_String.h
		BObject/BO_StringBuilder.c
		BObject/BO_StringBuilder.h
)

add_library(BFramework STATIC ${BFRAMEWORK_SOURCES})

target_include_directories(BFramework
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(BFramework BCore)