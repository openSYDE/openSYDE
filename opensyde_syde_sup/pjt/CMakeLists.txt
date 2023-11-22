# minimum 3.16 for precompiled headers
cmake_minimum_required(VERSION 3.16)

project("console_system_updater_sydesup")

#include script to generate PC-lint configuration files
include(lint_config.cmake)

if (NOT WIN32)
   #dynamic link
   set(OPENSSL_USE_STATIC_LIBS TRUE) 
   find_package(OpenSSL REQUIRED)
endif()

set(PROJECT_ROOT ${PROJECT_SOURCE_DIR}/..)

# --- PROJECT INCLUDE PATHS --------------------------------------------------------------------------------------------
set(SYDESUP_COMMON_INCLUDE_DIRECTORIES
   ${PROJECT_ROOT}/src
   )

if (WIN32)
   set(SYDESUP_COMMON_INCLUDE_DIRECTORIES
      ${SYDESUP_COMMON_INCLUDE_DIRECTORIES}
      #/libs/openssl/include needen only on windows
     ${PROJECT_ROOT}/libs/openssl/openssl/include
      )
endif()


include_directories(${SYDESUP_COMMON_INCLUDE_DIRECTORIES})

set(SYDESUP_LINUX_INCLUDE_DIRECTORIES
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux
   ${PROJECT_ROOT}/libs/opensyde_core/can_dispatcher/target_linux_socket_can
   ${PROJECT_ROOT}/libs/opensyde_core/ip_dispatcher/target_linux_sock
   )

# --- PROJECT SOURCE FILE PATHS ----------------------------------------------------------------------------------------
set(SYDESUP_COMMON_SOURCES
   ${PROJECT_ROOT}/src/C_SydeSup.cpp
   ${PROJECT_ROOT}/src/C_SupSuSequences.cpp
   ${PROJECT_ROOT}/src/C_SupCreatePackage.cpp
)

set(SYDESUP_WINDOWS_SOURCES
   ${PROJECT_ROOT}/src/C_SydeSupWindows.cpp
   ${PROJECT_ROOT}/src/resources.rc
   )

set(SYDESUP_LINUX_SOURCES
   ${PROJECT_ROOT}/src/C_SydeSupLinux.cpp
   ${PROJECT_ROOT}/src/resources.rc
   )

set(SYDESUP_LINUX_LIB_SOURCES
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TglFile.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TglTasks.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TglTime.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/kefex_diaglib/tgl_linux/TglUtils.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/ip_dispatcher/target_linux_sock/C_OscIpDispatcherLinuxSock.cpp
   ${PROJECT_ROOT}/libs/opensyde_core/can_dispatcher/target_linux_socket_can/C_Can.cpp
   )

# for Linux shared library only:
set(SYDESUP_API_HEADERS
   ${PROJECT_ROOT}/libs/opensyde_core/scl/C_SclString.hpp
   ${PROJECT_ROOT}/libs/opensyde_core/scl/C_SclDynamicArray.hpp
   ${PROJECT_ROOT}/src/C_SydeSup.hpp
   ${PROJECT_ROOT}/src/C_SydeSupLinux.hpp
)

set(SYDESUP_API_HEADERS_STWTYPES
   ${PROJECT_ROOT}/libs/opensyde_core/stwtypes/stwtypes.hpp
)

# --- COMPILER FLAGS AND LIBRARIES -------------------------------------------------------------------------------------

# OPENSYDE CORE
# only include relevant modules (include directories and source files)
set(OPENSYDE_CORE_SKIP_ZIPPING          0)
set(OPENSYDE_CORE_SKIP_IMPORTS          1)
set(OPENSYDE_CORE_SKIP_CODE_GENERATION  1)
set(OPENSYDE_CORE_SKIP_PROJECT_HANDLING 0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS_BASIC      1)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS_SYSTEM     0)
set(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS_COMMON     0)
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
   opensyde_core
   ws2_32   #WinSock
   Iphlpapi #IP helper API
   crypto
)

set(SYDESUP_LINK_LIBRARIES_LINUX
   pthread
   rt
   opensyde_core
   OpenSSL::Crypto
)

# COMPILER SWITCHES
# use same compiler switches that gcc uses when compiling openSYDE core from osc_moduletest.pro file
add_compile_options(-Wno-deprecated-declarations -Wall -Wextra)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

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
   link_directories(
       ../libs/openssl/openssl
   )
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
target_precompile_headers(SYDEsup PRIVATE ${PROJECT_ROOT}/src/precomp_headers.hpp)
target_precompile_headers(SYDEsup PRIVATE ${PROJECT_ROOT}/src/precomp_headers.hpp)

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
