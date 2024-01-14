cmake_minimum_required(VERSION 3.0.0)
project(project VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 17)

set(TargetName myTarget)
add_executable(${TargetName} src/main.cpp src/test.cpp)

target_link_libraries(${TargetName} PUBLIC CppUTest)
target_include_directories(${TargetName} PUBLIC inc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
add_test(NAME unitTest COMMAND test)