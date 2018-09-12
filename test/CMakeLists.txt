cmake_minimum_required(VERSION 3.2)

project(async_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

configure_file(in1.txt in1.txt COPYONLY)
configure_file(in2.txt in2.txt COPYONLY)
configure_file(in3.txt in3.txt COPYONLY)
configure_file(in4.txt in4.txt COPYONLY)
configure_file(in5.txt in5.txt COPYONLY)
configure_file(out1.txt out1.txt COPYONLY)
configure_file(out2.txt out2.txt COPYONLY)
configure_file(out3.txt out3.txt COPYONLY)
configure_file(out4.txt out4.txt COPYONLY)
configure_file(out5.txt out5.txt COPYONLY)

add_executable(async_test test_main.cpp ${HEADER_FILES})

set_target_properties(async_test PROPERTIES
INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS};${INCLUDE_DIR}"
)

target_link_libraries(async_test ${GTEST_LIBRARIES} )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_link_libraries(async_test pthread)
endif()

add_test(AsyncTest1 async_test)
add_test(AsyncTest2 async_test)
add_test(AsyncTest3 async_test)
add_test(AsyncTest4 async_test)
add_test(AsyncTest5 async_test)
