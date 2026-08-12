cmake_minimum_required(VERSION 3.20)

project(ElevatorSystem
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(elevator_system
    src/elevator.cpp
    src/building.cpp
    src/elevator_system.cpp
    src/dispatch_strategy.cpp
)

target_include_directories(elevator_system
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(elevator_system
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

add_executable(elevator_app
    src/main.cpp
)

target_link_libraries(elevator_app
    PRIVATE
        elevator_system
)

enable_testing()

add_executable(elevator_tests
    tests/elevator_system_test.cpp
)

target_link_libraries(elevator_tests
    PRIVATE
        elevator_system
)

add_test(
    NAME elevator_system_test
    COMMAND elevator_tests
)