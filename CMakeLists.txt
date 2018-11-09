cmake_minimum_required(VERSION 3.2)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
   project(async VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
   project(async VERSION 0.0.1)
endif()

find_package(GTest REQUIRED)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TEST_DATA_DIR ${PROJECT_SOURCE_DIR}/test_data)

file(GLOB HEADER_FILES	${INCLUDE_DIR}/*.h)

add_library(libasync SHARED async.cpp ${HEADER_FILES})
add_executable(async_test test_main.cpp ${HEADER_FILES})

set_target_properties(async_test PROPERTIES
	INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS};${INCLUDE_DIR}"
)

target_link_libraries(async_test libasync ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} )

set_target_properties(libasync PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	INCLUDE_DIRECTORIES ${INCLUDE_DIR}
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set_target_properties(libasync PROPERTIES COMPILE_OPTIONS -W3)
else()
	set_target_properties(libasync PROPERTIES COMPILE_OPTIONS -Wpedantic -Wall -Wextra)
	target_link_libraries(libasync pthread)
	target_link_libraries(async_test pthread)
endif()

configure_file(${TEST_DATA_DIR}/in1.txt in1.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/in1.txt in1.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/in2.txt in2.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/in3.txt in3.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/in4.txt in4.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/in5.txt in5.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/out1.txt out1.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/out2.txt out2.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/out3.txt out3.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/out4.txt out4.txt COPYONLY)
configure_file(${TEST_DATA_DIR}/out5.txt out5.txt COPYONLY)

enable_testing()

add_test(test1		async_test)
add_test(test2		async_test)
add_test(test3		async_test)
add_test(test4		async_test)
add_test(test5		async_test)
add_test(separated	async_test)
add_test(mike		async_test)

install(TARGETS libasync RUNTIME 
							DESTINATION bin 
							LIBRARY DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT a@a.a)

include (CPack)

