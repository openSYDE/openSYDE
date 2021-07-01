# minimum 3.16 for precompiled headers
cmake_minimum_required(VERSION 3.16)

project("osy_syde_coder_c")

# include script to generate PC-lint configuration files
include(lint_config.cmake)

# Set include directories before dragging in core; the core needs the "precomp_header.h" and files included by it
include_directories(
   ../src

   ../src/osy_code_exporter
   ../libs/osy_core/protocol_drivers   #just one header
)

# Drag in openSYDE core base (files, include paths)
# we don't need all packages
set(OPENSYDE_CORE_SKIP_ZIPPING          1)
set(OPENSYDE_CORE_SKIP_IMPORTS          1)
set(OPENSYDE_CORE_SKIP_CODE_GENERATION  0)
set(OPENSYDE_CORE_SKIP_PROJECT_HANDLING 0)
set(OPENSYDE_CORE_SKIP_PARAM_SET        0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS 1)
set(OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS  1)
set(OPENSYDE_CORE_SKIP_WINDOWS_TARGET   0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_LOGGING 1)
add_subdirectory(../libs/osy_core ${CMAKE_BINARY_DIR}/osy_core)

# --- Project source file ----------------------------------------------------------------------------------------------
set(OPENSYDE_SYDE_CODER_C_SOURCES
   ../src/C_CExport.cpp
   ../src/osy_code_exporter/C_OsyCodeExportBase.cpp

   ../src/resources.rc

   ../src/precomp_headers.h
   ../src/osy_code_exporter/C_OsyCodeExportBase.h
)

# define binary name and sources
add_executable(osy_syde_coder_c ${OPENSYDE_SYDE_CODER_C_SOURCES})

# add required libraries
target_link_libraries(osy_syde_coder_c
   opensyde_core
   version
)

# define precompiled headers for core and application
target_precompile_headers(opensyde_core PRIVATE ../src/precomp_headers.h)
target_precompile_headers(osy_syde_coder_c PRIVATE ../src/precomp_headers.h)

# create PC-lint configuration files
if(COMMAND add_pc_lint)
   add_pc_lint(osy_syde_coder_c ${OPENSYDE_SYDE_CODER_C_SOURCES})
endif(COMMAND add_pc_lint)

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

# Install results
install(TARGETS osy_syde_coder_c DESTINATION ${CMAKE_BINARY_DIR}/../result)
