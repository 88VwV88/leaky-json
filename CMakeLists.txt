cmake_minimum_required(VERSION 3.10)

project(LeakyJSON VERSION 1.0 LANGUAGES CXX)

find_package(Catch2 REQUIRED)

add_executable(leaky src/main.cpp src/parser.cpp)
set_target_properties(leaky PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_precompile_headers(leaky PUBLIC src/lexer.hpp src/parser.hpp)

add_executable(unit_tests test/main.cpp src/parser.cpp)
set_target_properties(unit_tests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_include_directories(unit_tests
    PUBLIC src/
    PUBLIC test/
)
target_link_libraries(unit_tests
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
enable_testing()
catch_discover_tests(unit_tests)