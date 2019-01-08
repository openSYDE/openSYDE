# dependencies
include(GenerateExportHeader)

# targets
add_library(${PROJECT_NAME} SHARED "")

# search paths
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src)
if(NOT OPTION_USE_CPP11_REGEX)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# sources/headers
target_sources(${PROJECT_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/Attribute.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeDefinition.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeRelation.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeValueType.h
        ${CMAKE_CURRENT_SOURCE_DIR}/BitTiming.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ByteOrder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentVariable.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedMultiplexor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/File.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Message.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Network.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Node.h
        ${CMAKE_CURRENT_SOURCE_DIR}/platform.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Signal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalGroup.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalType.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Status.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueDescriptions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueTable.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueType.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Attribute.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeDefinition.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeRelation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BitTiming.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentVariable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedMultiplexor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/File.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FileLoad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FileSave.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Message.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Network.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Node.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Signal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalGroup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalType.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueTable.cpp)

# generated files
configure_file(config.h.in config.h)
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
generate_export_header(${PROJECT_NAME})

# compiler/linker settings
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_VISIBILITY_PRESET "hidden"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    VISIBILITY_INLINES_HIDDEN 1)
if(CMAKE_COMPILER_IS_GNUCXX)
    #add_definitions(-pedantic)
    add_definitions(-D_FORTIFY_SOURCE=2)
    #add_definitions(-fPIE -pie)
    add_definitions(-Wl,-z,relro,-z,now)
    #if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    #    add_definitions(-fstack-protector-strong)
    #endif()
    if(OPTION_USE_GCOV_LCOV)
        add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
    endif()
endif()
if(NOT OPTION_USE_CPP11_REGEX)
    target_link_libraries(${PROJECT_NAME} ${Boost_REGEX_LIBRARY})
endif()
if(OPTION_USE_GCOV_LCOV)
    target_link_libraries(${PROJECT_NAME} gcov)
endif()

# install
install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/vector_dbc_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/config.h
        $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_SOURCES>
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Vector/DBC)

# sub directories
add_subdirectory(docs)
add_subdirectory(tests)
