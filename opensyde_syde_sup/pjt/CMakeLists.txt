# minimum 3.16 for precompiled headers
cmake_minimum_required(VERSION 3.16)

project("console_system_updater_sydesup")

#include script to generate PC-lint configuration files
include(lint_config.cmake)

set(PROJECT_ROOT ${PROJECT_SOURCE_DIR}/..)

# --- PROJECT INCLUDE PATHS --------------------------------------------------------------------------------------------
set(SYDESUP_COMMON_INCLUDE_DIRECTORIES
   ${PROJECT_ROOT}/src
   )

include_directories(${SYDESUP_COMMON_INCLUDE_DIRECTORIES})

set(SYDESUP_LINUX_INCLUDE_DIRECTORIES
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux
   ${PROJECT_ROOT}/libs/opensyde_core/can_dispatcher/target_linux_socket_can
   ${PROJECT_ROOT}/libs/opensyde_core/ip_dispatcher/target_linux_sock
   )

# --- PROJECT SOURCE FILE PATHS ----------------------------------------------------------------------------------------
set(SYDESUP_COMMON_SOURCES
   ${PROJECT_ROOT}/src/C_SYDEsup.cpp
   ${PROJECT_ROOT}/src/C_SUPSuSequences.cpp
)

set(SYDESUP_WINDOWS_SOURCES
   ${PROJECT_ROOT}/src/C_SYDEsupWindows.cpp
   ${PROJECT_ROOT}/src/resources.rc
   )

set(SYDESUP_LINUX_SOURCES
   ${PROJECT_ROOT}/src/C_SYDEsupLinux.cpp
   ${PROJECT_ROOT}/src/resources.rc
   )

set(SYDESUP_LINUX_LIB_SOURCES
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TGLFile.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TGLTasks.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TGLTime.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TGLUtils.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/ip_dispatcher/target_linux_sock/C_OSCIpDispatcherLinuxSock.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/can_dispatcher/target_linux_socket_can/CCAN.cpp
   )

# for Linux shared library only:
set(SYDESUP_API_HEADERS
   ${PROJECT_ROOT}/libs/opensyde_core/scl/CSCLString.h
   ${PROJECT_ROOT}/libs/opensyde_core/scl/SCLDynamicArray.h
   ${PROJECT_ROOT}/src/C_SYDEsup.h
   ${PROJECT_ROOT}/src/C_SYDEsupLinux.h
)

set(SYDESUP_API_HEADERS_STWTYPES
   ${PROJECT_ROOT}/libs/opensyde_core/stwtypes/stwtypes.h
)

# --- COMPILER FLAGS AND LIBRARIES -------------------------------------------------------------------------------------

# OPENSYDE CORE
# only include relevant modules (include directories and source files)
set(OPENSYDE_CORE_SKIP_ZIPPING          0)
set(OPENSYDE_CORE_SKIP_IMPORTS          1)
set(OPENSYDE_CORE_SKIP_CODE_GENERATION  1)
set(OPENSYDE_CORE_SKIP_PROJECT_HANDLING 0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS 0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_LOGGING 1)

if(WIN32)
   set(OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS  0)
   set(OPENSYDE_CORE_SKIP_WINDOWS_TARGET   0)
else()
   set(OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS  1)
   set(OPENSYDE_CORE_SKIP_WINDOWS_TARGET   1)
endif()

add_subdirectory(${PROJECT_ROOT}/libs/opensyde_core ${CMAKE_BINARY_DIR}/opensyde_core)

# SET LIBRARIES
set(SYDESUP_LINK_LIBRARIES_WINDOWS
   version
   ws2_32   #WinSock
   Iphlpapi #IP helper API
   opensyde_core
)

set(SYDESUP_LINK_LIBRARIES_LINUX
   pthread
   rt
   opensyde_core
)

# COMPILER SWITCHES
# use same compiler switches that gcc uses when compiling openSYDE core from osc_moduletest.pro file
add_compile_options(-Wno-deprecated-declarations -Wall -Wextra)

# do not warn about unused parameters in miniz.c; this library is not owned by us
set_source_files_properties(../libs/opensyde_core/miniz/miniz.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

# --- BUILD LIBRARY (LINUX ONLY) ---------------------------------------------------------------------------------------
if (NOT WIN32)
   include_directories(${SYDESUP_LINUX_INCLUDE_DIRECTORIES})
   add_library(sydesuplib SHARED ${SYDESUP_COMMON_SOURCES} ${SYDESUP_LINUX_SOURCES} ${SYDESUP_LINUX_LIB_SOURCES})
   target_link_libraries(sydesuplib ${SYDESUP_LINK_LIBRARIES_LINUX})
   target_include_directories(sydesuplib PRIVATE ${SYDESUP_COMMON_INCLUDE_DIRECTORIES}
                                          PRIVATE ${SYDESUP_LINUX_INCLUDE_DIRECTORIES})
   target_include_directories(opensyde_core PRIVATE ${SYDESUP_LINUX_INCLUDE_DIRECTORIES})
   # -fvisibility=hidden: Export only the functions that are explicitly declared as API functions (see macro LIB_PUBLIC)
   # -fPIC: position independent code required to build a shared library
   target_compile_options(sydesuplib PUBLIC -fPIC -fvisibility=hidden)
endif()

# --- BUILD EXECUTABLE -------------------------------------------------------------------------------------------------
if(WIN32)
   add_executable(SYDEsup ${SYDESUP_COMMON_SOURCES} ${SYDESUP_WINDOWS_SOURCES} ${PROJECT_ROOT}/src/WindowsMain.cpp)
   target_link_libraries(SYDEsup ${SYDESUP_LINK_LIBRARIES_WINDOWS})
else()
   include_directories(${SYDESUP_LINUX_INCLUDE_DIRECTORIES})
   add_executable(SYDEsup ${SYDESUP_COMMON_SOURCES} ${SYDESUP_LINUX_SOURCES} ${SYDESUP_LINUX_LIB_SOURCES}
                          ${PROJECT_ROOT}/src/LinuxMain.cpp)
   target_link_libraries(SYDEsup ${SYDESUP_LINK_LIBRARIES_LINUX})
   target_include_directories(SYDEsup PRIVATE ${SYDESUP_COMMON_INCLUDE_DIRECTORIES}
                                      PRIVATE ${SYDESUP_LINUX_INCLUDE_DIRECTORIES})
   target_include_directories(opensyde_core PRIVATE ${SYDESUP_LINUX_INCLUDE_DIRECTORIES})
endif()

# common include directories
target_include_directories(SYDEsup PRIVATE ${SYDESUP_COMMON_INCLUDE_DIRECTORIES})

# precompiled headers for core and application
target_precompile_headers(SYDEsup PRIVATE ${PROJECT_ROOT}/src/precomp_headers.h)
target_precompile_headers(SYDEsup PRIVATE ${PROJECT_ROOT}/src/precomp_headers.h)

# create PC-lint configuration files
if(COMMAND add_pc_lint)
   if(WIN32)
		add_pc_lint(SYDEsup ${SYDESUP_COMMON_SOURCES} ${SYDESUP_WINDOWS_SOURCES} ${PROJECT_ROOT}/src/WindowsMain.cpp)
   else()
		add_pc_lint(SYDEsup ${SYDESUP_COMMON_SOURCES} ${SYDESUP_LINUX_SOURCES} ${PROJECT_ROOT}/src/LinuxMain.cpp)
   endif(WIN32)						  
endif(COMMAND add_pc_lint)

# --- INSTALL RESULTS --------------------------------------------------------------------------------------------------
install(TARGETS SYDEsup DESTINATION ${PROJECT_ROOT}/result/${INSTALL_SUBDIR})
if (NOT WIN32)
   install(TARGETS sydesuplib DESTINATION ${PROJECT_ROOT}/result/${INSTALL_SUBDIR}/sydesup_lib)
   install(FILES ${SYDESUP_API_HEADERS} DESTINATION ${PROJECT_ROOT}/result/${INSTALL_SUBDIR}/sydesup_lib)
   install(FILES ${SYDESUP_API_HEADERS_STWTYPES} DESTINATION ${PROJECT_ROOT}/result/${INSTALL_SUBDIR}/sydesup_lib/stwtypes)
endif()
