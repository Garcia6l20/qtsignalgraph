cmake_minimum_required(VERSION 2.8)

project(qtsignalgraph CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME}
    include/details/function_traits.hpp

    include/QSignalDetails.hpp

    include/QSignalGraph.hpp
    src/QSignalGraph.cpp

    include/QSignalSource.hpp
    src/QSignalSource.cpp

    include/QSignalBaseJunction.hpp
    include/QSignalBaseJunction.tpp

    include/QSignalBinJunction.hpp
    include/QSignalBinJunction.tpp
    src/QSignalBinJunction.cpp

    include/QSignalConjunction.hpp
    include/QSignalConjunction.tpp
    src/QSignalConjunction.cpp

    include/QSignalDisjunction.hpp
    include/QSignalDisjunction.tpp
    src/QSignalDisjunction.cpp
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
    enable_testing()
    include(CTest)
    add_subdirectory(samples)
endif()
