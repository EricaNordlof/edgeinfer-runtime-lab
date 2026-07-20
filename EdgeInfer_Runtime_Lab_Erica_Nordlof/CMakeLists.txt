cmake_minimum_required(VERSION 3.16)
project(edgeinfer_runtime_lab LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(edgeinfer STATIC
    src/model.cpp
    src/backends.cpp
    src/runtime.cpp
    src/c_api.cpp
    src/linux_metrics.cpp
)
target_include_directories(edgeinfer PUBLIC include)
target_compile_options(edgeinfer PRIVATE -Wall -Wextra -Wpedantic)

add_executable(edgeinfer_bench apps/benchmark.cpp)
target_link_libraries(edgeinfer_bench PRIVATE edgeinfer)

add_executable(edgeinfer_service apps/service.cpp)
target_link_libraries(edgeinfer_service PRIVATE edgeinfer)

enable_testing()
add_executable(edgeinfer_tests tests/test_runtime.cpp)
target_link_libraries(edgeinfer_tests PRIVATE edgeinfer)
add_test(NAME edgeinfer_tests COMMAND edgeinfer_tests ${CMAKE_SOURCE_DIR}/models/tinynet.eir)
