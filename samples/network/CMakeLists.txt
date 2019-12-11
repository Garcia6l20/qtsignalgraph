project(${PROJECT_NAME}-network)

find_package(Qt5 COMPONENTS Network REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Network)
