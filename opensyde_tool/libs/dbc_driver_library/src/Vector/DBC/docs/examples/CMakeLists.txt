include_directories(${PROJECT_SOURCE_DIR}/src)

if(OPTION_BUILD_EXAMPLES)
    add_definitions(
        -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        -DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

    set(CMAKE_STATIC_LIBRARY_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
    set(CMAKE_SHARED_LIBRARY_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")

    if(WIN32)
        add_custom_target(Vector_DBC-Copy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${PROJECT_BINARY_DIR}/src/Vector/DBC/${PROJECT_NAME}.dll"
                "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Copy ${PROJECT_BINARY_DIR}/src/Vector/DBC/${PROJECT_NAME}.dll to ${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    add_executable(example_List_Messages_Signals List_Messages_Signals.cpp)
    add_executable(example_Message_Encode_Decode Message_Encode_Decode.cpp)

    target_link_libraries(example_List_Messages_Signals ${PROJECT_NAME})
    target_link_libraries(example_Message_Encode_Decode ${PROJECT_NAME})
endif()

install(
    FILES
        List_Messages_Signals.cpp
        Message_Encode_Decode.cpp
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples)
