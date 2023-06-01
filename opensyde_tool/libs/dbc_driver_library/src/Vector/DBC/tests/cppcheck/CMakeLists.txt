get_target_property(sources ${PROJECT_NAME} SOURCES)

add_custom_target(cppcheck_${PROJECT_NAME} ALL
    COMMAND
        ${CPPCHECK_EXECUTABLE}
        --enable=all
        -I "${PROJECT_BINARY_DIR}/src/Vector_DBC"
        --output-file=cppcheck-err.xml
        --suppressions-list=${CMAKE_CURRENT_SOURCE_DIR}/suppressions.txt
        --xml
        --xml-version=2
        ${sources}
        > cppcheck-out.txt
    DEPENDS
        ${sources}
    BYPRODUCTS
        cppcheck-out.txt
        cppcheck-err.xml
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/suppressions.txt)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cppcheck-out.txt
        ${CMAKE_CURRENT_BINARY_DIR}/cppcheck-err.xml
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/cppcheck)
