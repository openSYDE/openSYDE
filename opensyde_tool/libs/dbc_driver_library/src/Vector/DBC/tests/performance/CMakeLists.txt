include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(performance_test performance_test.cpp)

target_link_libraries(performance_test
    ${PROJECT_NAME})
