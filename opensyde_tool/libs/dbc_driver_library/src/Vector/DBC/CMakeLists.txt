# dependencies
include(GenerateExportHeader)

# targets
add_library(${PROJECT_NAME} SHARED "")

# search paths
include_directories(
    ${FLEX_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src)

# sources/headers
target_sources(${PROJECT_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/Attribute.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeDefinition.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeObjectType.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeRelation.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeValueType.h
        ${CMAKE_CURRENT_SOURCE_DIR}/BitTiming.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ByteOrder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentVariable.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedMultiplexor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Message.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Network.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Node.h
        ${CMAKE_CURRENT_SOURCE_DIR}/platform.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Signal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalGroup.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalType.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueDescriptions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueTable.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueType.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeDefinition.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeRelation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeValueType.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BitTiming.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentVariable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Message.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Network.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/platform.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Signal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalGroup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalType.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ValueTable.cpp)

# generated files
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
generate_export_header(${PROJECT_NAME})
flex_target(Scanner Scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/Scanner.cpp
    COMPILE_FLAGS --never-interactive)
bison_target(Parser Parser.yy ${CMAKE_CURRENT_BINARY_DIR}/Parser.cpp
    VERBOSE)
add_flex_bison_dependency(Scanner Parser)
target_sources(${PROJECT_NAME}
    PRIVATE
        ${FLEX_Scanner_OUTPUTS}
        ${BISON_Parser_OUTPUTS})

# compiler/linker settings
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_VISIBILITY_PRESET "hidden"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    VISIBILITY_INLINES_HIDDEN 1)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-pedantic)
    add_definitions(-D_FORTIFY_SOURCE=2)
    #add_definitions(-fPIE -pie)
    add_definitions(-Wl,-z,relro,-z,now)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
        add_definitions(-fstack-protector-strong)
    endif()
    if(OPTION_USE_GCOV)
        add_definitions(-g -O0 -fprofile-arcs -ftest-coverage)
    endif()
    if(OPTION_USE_GPROF)
         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
         set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
         set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
     endif()
endif()
if(OPTION_USE_GCOV)
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
        $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_SOURCES>
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Vector/DBC)

# sub directories
add_subdirectory(docs)
add_subdirectory(tests)
