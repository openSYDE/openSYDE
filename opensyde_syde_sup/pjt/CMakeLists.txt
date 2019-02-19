cmake_minimum_required(VERSION 2.8)
project("console_system_updater_sydesup")


set(PROJECT_ROOT ${PROJECT_SOURCE_DIR}/..)

# --- Project source file ----------------------------------------------------------------------------------------------
set(SYDESUP_COMMON_SOURCES
   ../src/main.cpp
   ../libs/opensyde_core/C_OSCUtils.cpp
	../libs/opensyde_core/C_OSCZipFile.cpp
   ../libs/opensyde_core/can_dispatcher/dispatcher/CCANBase.cpp
   ../libs/opensyde_core/can_dispatcher/dispatcher/CCANDispatcher.cpp
   ../libs/opensyde_core/kefex_diaglib/stwcompid.c
   ../libs/opensyde_core/kefex_diaglib/DLStrings.cpp
   ../libs/opensyde_core/kefex_diaglib/CXFLActions.cpp
   ../libs/opensyde_core/kefex_diaglib/CXFLHexFile.cpp
   ../libs/opensyde_core/kefex_diaglib/CXFLProtocol.cpp
   ../libs/opensyde_core/kefex_diaglib/CXFLFlashWrite.cpp
   ../libs/opensyde_core/kefex_diaglib/CXFLFlashWriteParameters.cpp
   ../libs/opensyde_core/kefex_diaglib/CXFLECUInformation.cpp
   ../libs/opensyde_core/kefex_diaglib/CDLReportEvents.cpp
   ../libs/opensyde_core/kefex_diaglib/CHexFile.cpp
   ../libs/opensyde_core/kefex_diaglib/CSCLResourceStrings.cpp
   ../libs/opensyde_core/logging/C_OSCLoggingHandler.cpp
   ../libs/opensyde_core/miniz/miniz.c
   ../libs/opensyde_core/project/system/C_OSCSystemDefinitionFiler.cpp
   ../libs/opensyde_core/project/system/C_OSCSystemBus.cpp
   ../libs/opensyde_core/project/system/C_OSCSystemBusFiler.cpp
   ../libs/opensyde_core/project/system/C_OSCDeviceManager.cpp
   ../libs/opensyde_core/project/system/C_OSCSystemDefinition.cpp
   ../libs/opensyde_core/project/system/C_OSCSystemFilerUtil.cpp
   ../libs/opensyde_core/project/system/C_OSCDeviceGroup.cpp
   ../libs/opensyde_core/project/system/C_OSCDeviceDefinition.cpp
   ../libs/opensyde_core/project/system/C_OSCDeviceDefinitionFiler.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNode.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeFiler.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeProperties.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeComInterfaceSettings.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeDataPool.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolFiler.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolContent.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolDataSet.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListElement.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolList.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeApplication.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeOpenSydeServerSettings.cpp
   ../libs/opensyde_core/project/system/node/C_OSCNodeStwFlashloaderSettings.cpp
   ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageContainer.cpp
   ../libs/opensyde_core/project/system/node/can/C_OSCCanProtocol.cpp
   ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageIdentificationIndices.cpp
   ../libs/opensyde_core/project/system/node/can/C_OSCCanMessage.cpp
   ../libs/opensyde_core/project/system/node/can/C_OSCCanSignal.cpp
   ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsy.cpp
   ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpBase.cpp
   ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpCan.cpp
   ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpIp.cpp
   ../libs/opensyde_core/protocol_drivers/C_OSCCanDispatcherOsyRouter.cpp
   ../libs/opensyde_core/protocol_drivers/C_OSCFlashProtocolStwFlashloader.cpp
   ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverBase.cpp
   ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverProtocol.cpp
   ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverFlash.cpp
   ../libs/opensyde_core/protocol_drivers/communication/C_OSCComSequencesBase.cpp
   ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingCalculation.cpp
   ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingRoute.cpp
   ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.cpp
   ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuSequences.cpp
   ../libs/opensyde_core/protocol_drivers/system_update/C_OsyHexFile.cpp
   ../libs/opensyde_core/scl/CSCLChecksums.cpp
   ../libs/opensyde_core/scl/CSCLString.cpp
   ../libs/opensyde_core/scl/CSCLStringList.cpp
   ../libs/opensyde_core/scl/CSCLDateTime.cpp
   ../libs/opensyde_core/scl/CSCLIniFile.cpp
   ../libs/opensyde_core/xml_parser/C_OSCXMLParser.cpp
   ../libs/opensyde_core/xml_parser/tinyxml2/tinyxml2.cpp
   ../src/C_SYDEsup.cpp
   ../src/C_SUPSuSequences.cpp
)

set(SYDESUP_TGL_SOURCES
   ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLFile.cpp
   ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTasks.cpp
   ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTime.cpp
   ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLUtils.cpp
   ../libs/opensyde_core/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.cpp
   ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCAN.cpp
   ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCANDLL.cpp
)


# --- Project include pathes -------------------------------------------------------------------------------------------
set(SYDESUP_COMMON_INCLUDE_DIRECTORIES
   ../src
   ../libs/opensyde_core
   ../libs/opensyde_core/can_dispatcher
   ../libs/opensyde_core/can_dispatcher/dispatcher
   ../libs/opensyde_core/ip_dispatcher/dispatcher
   ../libs/opensyde_core/data_dealer
   ../libs/opensyde_core/data_dealer/paramset
   ../libs/opensyde_core/exports
   ../libs/opensyde_core/imports
   ../libs/opensyde_core/logging
   ../libs/opensyde_core/md5
   ../libs/opensyde_core/project
   ../libs/opensyde_core/project/system
   ../libs/opensyde_core/project/system/node
   ../libs/opensyde_core/project/system/node/can
   ../libs/opensyde_core/protocol_drivers
   ../libs/opensyde_core/protocol_drivers/routing
   ../libs/opensyde_core/protocol_drivers/communication
   ../libs/opensyde_core/protocol_drivers/system_update
   ../libs/opensyde_core/scl
   ../libs/opensyde_core/stwtypes
   ../libs/opensyde_core/kefex_diaglib
   ../libs/opensyde_core/xml_parser
   ../libs/opensyde_core/xml_parser/tinyxml2
   ../libs/opensyde_core/miniz
)

set(SYDESUP_TGL_INCLUDE_DIRECTORIES
   ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls
   ../libs/opensyde_core/ip_dispatcher/target_windows_win_sock
   ../libs/opensyde_core/kefex_diaglib/tgl_windows
)


# --- Project link libraries -------------------------------------------------------------------------------------------
set(SYDESUP_LINK_LIBRARIES
   version
   ws2_32   #WinSock
   Iphlpapi #IP helper API
)


# --- Compiler flags ---------------------------------------------------------------------------------------------------
#link all libraries statically
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Os -static -static-libgcc -static-libstdc++")


# Build flashloader executable
add_executable(sydesup ${SYDESUP_COMMON_SOURCES} ${SYDESUP_TGL_SOURCES})
target_include_directories(sydesup PRIVATE ${SYDESUP_COMMON_INCLUDE_DIRECTORIES}
                                   PRIVATE ${SYDESUP_TGL_INCLUDE_DIRECTORIES})
target_link_libraries(sydesup ${SYDESUP_LINK_LIBRARIES})


# Install results
install(TARGETS sydesup DESTINATION ${PROJECT_ROOT}/result)
