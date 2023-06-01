cmake_minimum_required(VERSION 3.9)

project(Vector_DBC
    VERSION 2.0.6
    DESCRIPTION "Vector DBC")

# build types: None, Debug, Release, RelWithDebInfo, MinSizeRel
set(CMAKE_BUILD_TYPE Release)

# source code documentation
option(OPTION_RUN_DOXYGEN "Run Doxygen" ON)

# static code analysis
option(OPTION_RUN_CCCC "Run CCCC" OFF)
option(OPTION_RUN_CPPCHECK "Run Cppcheck" OFF)

# dynamic tests
option(OPTION_BUILD_EXAMPLES "Build examples" OFF)
option(OPTION_BUILD_TESTS "Build tests" OFF)
option(OPTION_USE_GCOV "Build with gcov to generate coverage data on execution" OFF)
option(OPTION_USE_GPROF "Build with gprof" OFF)
option(OPTION_ADD_LCOV "Add lcov targets to generate HTML coverage report" OFF)

# directories
include(GNUInstallDirs)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# dependencies
find_package(FLEX REQUIRED)
find_package(BISON 3.3 REQUIRED)
if(OPTION_RUN_DOXYGEN)
    find_package(Doxygen REQUIRED)
    find_package(Graphviz)
    if(WIN32)
        find_package(HTMLHelp REQUIRED)
    endif()
endif()
if(OPTION_RUN_CCCC)
    find_package(CCCC REQUIRED)
endif()
if(OPTION_RUN_CPPCHECK)
    find_package(Cppcheck REQUIRED)
endif()
if(OPTION_BUILD_TESTS)
    enable_testing()
    find_package(Boost 1.55 COMPONENTS system filesystem unit_test_framework REQUIRED)
endif()
if(OPTION_ADD_LCOV)
    find_package(LCOV REQUIRED)
endif()

# coverage
if(OPTION_USE_GCOV)
    add_custom_target(gcov_clean
        COMMAND find ${PROJECT_BINARY_DIR} -name '*.gcda' -delete)
endif()
if(OPTION_ADD_LCOV)
    add_custom_target(lcov_capture
        COMMAND ${LCOV_EXECUTABLE}
            --capture
            --directory ${PROJECT_BINARY_DIR}
            --output-file coverage.info)

    add_custom_target(lcov_genhtml
        COMMAND ${LCOV_GENHTML_EXECUTABLE}
            --output-directory lcov
            coverage.info
        DEPENDS lcov_capture)

    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lcov
        DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()

# package
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "Tobias Lorenz <tobias.lorenz@gmx.net>")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.GPL-3.0)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
include(CPack)

# install
install(
    FILES
        CHANGELOG.md
        README.md
        LICENSE.GPL-3.0
    DESTINATION ${CMAKE_INSTALL_DOCDIR})

# sub directories
add_subdirectory(src)
