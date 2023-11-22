# minimum 3.16 for precompiled headers
cmake_minimum_required(VERSION 3.16)

project("osy_cmd_line_flash_tool")

#include script to generate PC-lint configuration files
include(lint_config.cmake)

set(PROJECT_ROOT ${PROJECT_SOURCE_DIR}/..)

# --- PROJECT INCLUDE PATHS --------------------------------------------------------------------------------------------
# Set include directories before dragging in core; the core needs the "precomp_headers.hpp" and files included by it
include_directories(
   ${PROJECT_ROOT}/src
)

# --- COMPILER FLAGS AND LIBRARIES -------------------------------------------------------------------------------------

# OPENSYDE CORE
# only include relevant modules (includes directories and source files)
set(OPENSYDE_CORE_SKIP_ZIPPING                     1)
set(OPENSYDE_CORE_SKIP_IMPORTS                     1)
set(OPENSYDE_CORE_SKIP_CODE_GENERATION             1)
set(OPENSYDE_CORE_SKIP_PROJECT_HANDLING            1)
set(OPENSYDE_CORE_SKIP_PARAM_SET                   1)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS_BASIC      0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS_SYSTEM     1)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS_COMMON     0)
set(OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS             0)
set(OPENSYDE_CORE_SKIP_WINDOWS_TARGET              0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_LOGGING            1)
add_subdirectory(${PROJECT_ROOT}/libs/osy_core ${CMAKE_BINARY_DIR}/osy_core)

# warn a lot
# Unfortunately forcing C90 by using "-Wc90-c99-compat" or "-std=c89" would (correctly) also prohibit
#  C++ style comments and there's no switch to explicitly suppress this warning
set(CMAKE_C_FLAGS "-Wall -Wextra -Wc99-c11-compat -Wunused -Wuninitialized -Wmaybe-uninitialized \
   -Wdeclaration-after-statement -Wsign-compare -Wstrict-prototypes -Wmissing-prototypes \
   -Wdouble-promotion -Wfloat-equal -Wmissing-include-dirs -Wshadow -Wundef \
   -Wunused-macros -Wcast-align -Wlogical-op -Wno-multichar -Wredundant-decls -Wnested-externs")
# For C++ do not add "-Wdouble-promotion" as it causes a warning in tinyxml.cpp which we do not want to modify
set(CMAKE_CXX_FLAGS "-std=c++98 -Wall -Wextra -Wunused -Wuninitialized -Wmaybe-uninitialized -Wsign-compare  \
   -Wmissing-include-dirs -Wshadow -Wlogical-op -Wno-multichar -Wredundant-decls")

# link standard libraries statically
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

# --- Project source file ----------------------------------------------------------------------------------------------
set(OSY_CMD_LINE_FLASH_TOOL_SOURCES
   ${PROJECT_ROOT}/src/main.cpp
   ${PROJECT_ROOT}/src/C_BasicFlashTool.cpp
   ${PROJECT_ROOT}/src/C_BasicUpdateSequence.cpp

   ${PROJECT_ROOT}/src/C_BasicFlashTool.hpp
   ${PROJECT_ROOT}/src/C_BasicUpdateSequence.hpp

   ../src/resources.rc
)

# --- BUILD EXECUTABLE -------------------------------------------------------------------------------------------------

# define binary name and sources
add_executable(osy_cmd_line_flash_tool ${OSY_CMD_LINE_FLASH_TOOL_SOURCES})

# add required libraries
target_link_libraries(osy_cmd_line_flash_tool
   opensyde_core
   version
)

# define precompiled headers for core and application
target_precompile_headers(opensyde_core PRIVATE ${PROJECT_ROOT}/src/precomp_headers.hpp)
target_precompile_headers(osy_cmd_line_flash_tool PRIVATE ${PROJECT_ROOT}/src/precomp_headers.hpp)

# create PC-lint configuration files
if(COMMAND add_pc_lint)
   add_pc_lint(osy_cmd_line_flash_tool ${OSY_CMD_LINE_FLASH_TOOL_SOURCES})
endif(COMMAND add_pc_lint)

# --- INSTALL RESULTS --------------------------------------------------------------------------------------------------
install(TARGETS osy_cmd_line_flash_tool DESTINATION ${PROJECT_ROOT}/result)
