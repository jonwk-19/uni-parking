# Catch2 tests
# If Catch2 is not installed: download single header from https://github.com/catchorg/Catch2/releases

# Include Catch2 (assumes single header in tests/catch.hpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Compile sources without main.cpp for testing
file(GLOB TEST_SOURCES "../src/*.cpp")
list(REMOVE_ITEM TEST_SOURCES "../src/main.cpp")

# Test executable
add_executable(RunTests
    test_parqueo.cpp
    test_validaciones.cpp
    ${TEST_SOURCES}
)
target_include_directories(RunTests PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME ParqueoTests COMMAND RunTests)
