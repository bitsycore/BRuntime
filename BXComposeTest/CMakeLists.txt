# BComposeTest - Demo Application

add_executable(BComposeTest
    main.c
)

target_include_directories(BComposeTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(BComposeTest
    BCompose
    BFramework
    BCore
)
