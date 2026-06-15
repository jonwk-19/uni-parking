cmake_minimum_required(VERSION 3.15)
project(SistemaParqueo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
file(GLOB SOURCES "src/*.cpp" "main.cpp")
add_executable(SistemaParqueo ${SOURCES})
target_include_directories(SistemaParqueo PRIVATE ${CMAKE_SOURCE_DIR}/include)

# MySQL support (optional)
if(USAR_MYSQL)
    target_compile_definitions(SistemaParqueo PRIVATE USAR_MYSQL)
    find_package(MySQL REQUIRED)
    target_link_libraries(SistemaParqueo PRIVATE ${MYSQL_LIBRARIES})
    target_include_directories(SistemaParqueo PRIVATE ${MYSQL_INCLUDE_DIR})
endif()

# Enable testing
enable_testing()

# Tests (optional, only if tests directory exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()
