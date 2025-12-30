include(GoogleTest)

#add_subdirectory(..)

set(test_name test_simple_module)

add_executable(${test_name} ${test_name}.cc)
string(REGEX REPLACE "^test_" "" cut ${test_name})
target_link_libraries(${test_name}
    ${cut}
    GTest::gtest_main
    )
target_include_directories(${test_name}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/modules/${cut}
    )
if(test_flags STREQUAL " ")
else()
    target_link_libraries(${test_name}
        ${test_flags}
        )
endif()
gtest_discover_tests(${test_name})