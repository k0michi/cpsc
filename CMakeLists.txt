cmake_minimum_required(VERSION 3.20)
project(cpsc_snippets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
file(GLOB SNIPPET_TEST_SOURCES CONFIGURE_DEPENDS snippets/*.test.cc)
add_executable(snippet_tests ${SNIPPET_TEST_SOURCES})
target_include_directories(snippet_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snippet_tests PRIVATE GTest::gtest_main)

if(MSVC)
  target_compile_options(snippet_tests PRIVATE /W4)
else()
  target_compile_options(snippet_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GoogleTest)
gtest_discover_tests(snippet_tests)
