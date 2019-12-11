cmake_minimum_required(VERSION 2.8)

project(qtsignalgraph CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(${PROJECT_NAME}_headers
    include/QSignalGraph.hpp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# qt5_wrap_cpp(${PROJECT_NAME}_mocs ${${PROJECT_NAME}_headers})

add_library(${PROJECT_NAME}
    include/QSignalGraph.hpp
    include/QSignalConjunction.hpp
    include/QSignalSource.hpp
    src/QSignalGraph.cpp
    src/QSignalConjunction.cpp
    src/QSignalSource.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)
target_compile_definitions(${PROJECT_NAME} PUBLIC QT_NO_FOREACH=1)

option(QT_SIGNALGRAPH_TESTS_EN "Enable ${PROJECT_NAME} testing" ON)
if(QT_SIGNALGRAPH_TESTS_EN)
    add_subdirectory(tests)
endif()

option(QT_SIGNALGRAPH_SAMPLES_EN "Enable ${PROJECT_NAME} samples" ON)
if(QT_SIGNALGRAPH_SAMPLES_EN)
    add_subdirectory(samples)
endif()
