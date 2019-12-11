find_package(Qt5 COMPONENTS Core Test REQUIRED)
link_libraries(${PROJECT_NAME} Qt5::Test)

enable_testing()

project(${PROJECT_NAME}-tests)
include_directories(${PROJECT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)

#qt5_wrap_cpp(_mocs TestObject.hpp)
set(_test_object_src TestObject.hpp TestObject.cpp)

#qt5_wrap_cpp(_test-basic-moc basic-test.cpp OPTIONS -i -o ${PROJECT_BINARY_DIR}/basic-test.moc)

add_executable(${PROJECT_NAME}-basic basic-test.cpp ${_test_object_src} ${_mocs})
target_include_directories(${PROJECT_NAME}-basic PRIVATE ${PROJECT_BINARY_DIR})
add_test(NAME ${PROJECT_NAME}-basic COMMAND ${PROJECT_NAME}-basic)
#add_dependencies(${PROJECT_NAME}-basic ${_test-basic-moc})

include(CTest)
