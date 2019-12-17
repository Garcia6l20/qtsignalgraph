find_package(Qt5 COMPONENTS Core Test REQUIRED)
link_libraries(${PROJECT_NAME} Qt5::Test)

include_directories(${PROJECT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)

set(_test_object_src TestObject.hpp TestObject.cpp)

macro(graph_test _file)
    get_filename_component(_target ${_file} NAME_WE)
    set(_target "${PROJECT_NAME}-${_target}")
    add_executable(${_target} ${_file} ${_test_object_src})
    target_include_directories(${_target} PRIVATE ${PROJECT_BINARY_DIR})
    add_test(NAME ${_target} COMMAND ${_target})
endmacro()

graph_test(basic-test.cpp)
graph_test(conjunction-test.cpp)
graph_test(disjunction-test.cpp)
graph_test(binary-junction-test.cpp)
graph_test(argforwarding-test.cpp)
graph_test(traits-test.cpp)
