TARGET = SYDEsup
TEMPLATE = app
CONFIG += console

#no Qt stuff at all (pure ANSI-C++)
QT -= core
QT -= gui

#add windows API libraries
LIBS += -lversion
LIBS += -lws2_32   #WinSock
LIBS += -lIphlpapi #IP helper API

#link all libraries statically
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++

#output directory
DESTDIR = ../result

#version info:
RC_FILE = ../src/resources.rc

SOURCES += ../src/main.cpp \
           ../libs/opensyde_core/C_OSCUtils.cpp \
           ../libs/opensyde_core/C_OSCZipFile.cpp \
           ../libs/opensyde_core/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.cpp \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANBase.cpp \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANDispatcher.cpp \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCAN.cpp \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCANDLL.cpp \
           ../libs/opensyde_core/kefex_diaglib/stwcompid.c \
           ../libs/opensyde_core/kefex_diaglib/DLStrings.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLActions.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLHexFile.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLProtocol.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWrite.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWriteParameters.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLECUInformation.cpp \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLFile.cpp \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTasks.cpp \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTime.cpp \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLUtils.cpp \
           ../libs/opensyde_core/kefex_diaglib/CDLReportEvents.cpp \
           ../libs/opensyde_core/kefex_diaglib/CHexFile.cpp \
           ../libs/opensyde_core/kefex_diaglib/CSCLResourceStrings.cpp \
           ../libs/opensyde_core/logging/C_OSCLoggingHandler.cpp \
           ../libs/opensyde_core/miniz/miniz.c \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinitionFiler.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemBus.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemBusFiler.cpp \
           ../libs/opensyde_core/project/system/C_OSCDeviceManager.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinition.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemFilerUtil.cpp \
           ../libs/opensyde_core/project/system/C_OSCDeviceGroup.cpp \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinition.cpp \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinitionFiler.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNode.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeFiler.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeProperties.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeComInterfaceSettings.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPool.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolFiler.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolContent.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolDataset.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListElement.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolList.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeApplication.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeOpenSydeServerSettings.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeStwFlashloaderSettings.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageContainer.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanProtocol.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageIdentificationIndices.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessage.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanSignal.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsy.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpBase.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpCan.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpIp.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCCanDispatcherOsyRouter.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCFlashProtocolStwFlashloader.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverBase.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverProtocol.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverFlash.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComSequencesBase.cpp \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingCalculation.cpp \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingRoute.cpp \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.cpp \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuSequences.cpp \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OsyHexFile.cpp \
           ../libs/opensyde_core/scl/CSCLChecksums.cpp \
           ../libs/opensyde_core/scl/CSCLString.cpp \
           ../libs/opensyde_core/scl/CSCLStringList.cpp \
           ../libs/opensyde_core/scl/CSCLDateTime.cpp \
           ../libs/opensyde_core/scl/CSCLIniFile.cpp \
           ../libs/opensyde_core/xml_parser/C_OSCXMLParser.cpp \
           ../libs/opensyde_core/xml_parser/tinyxml2/tinyxml2.cpp \
           ../src/C_SYDEsup.cpp \
           ../src/C_SUPSuSequences.cpp

HEADERS += ../libs/opensyde_core/scl/CSCLString.h \
           ../libs/opensyde_core/C_OSCUtils.h \
           ../libs/opensyde_core/ip_dispatcher/dispatcher/C_OSCIpDispatcher.h \
           ../libs/opensyde_core/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.h \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANBase.h \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANDispatcher.h \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCAN.h \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCANDLL.h \
           ../libs/opensyde_core/kefex_diaglib/stwcompid.c \
           ../libs/opensyde_core/kefex_diaglib/DLStrings.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLActions.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLHexFile.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLProtocol.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWrite.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWriteParameters.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLECUInformation.h \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLFile.h \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTasks.h \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTime.h \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLUtils.h \
           ../libs/opensyde_core/kefex_diaglib/CDLReportEvents.h \
           ../libs/opensyde_core/kefex_diaglib/CHexFile.h \
           ../libs/opensyde_core/kefex_diaglib/CSCLResourceStrings.h \
           ../libs/opensyde_core/logging/C_OSCLoggingHandler.h \
           ../libs/opensyde_core/miniz/miniz.c \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinitionFiler.h \
           ../libs/opensyde_core/project/system/C_OSCSystemBus.h \
           ../libs/opensyde_core/project/system/C_OSCSystemBusFiler.h \
           ../libs/opensyde_core/project/system/C_OSCDeviceManager.h \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinition.h \
           ../libs/opensyde_core/project/system/C_OSCSystemFilerUtil.h \
           ../libs/opensyde_core/project/system/C_OSCDeviceGroup.h \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinition.h \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinitionFiler.h \
           ../libs/opensyde_core/project/system/node/C_OSCNode.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeFiler.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeProperties.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeComInterfaceSettings.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPool.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolFiler.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolContent.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolDataset.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListElement.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolList.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolExportSettings.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeApplication.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeOpenSydeServerSettings.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeStwFlashloaderSettings.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageContainer.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanProtocol.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageIdentificationIndices.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessage.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanSignal.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsy.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpBase.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpCan.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpIp.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCCanDispatcherOsyRouter.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCFlashProtocolStwFlashloader.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverBase.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverProtocol.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverFlash.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComSequencesBase.h \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingCalculation.h \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingRoute.h \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.h \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuSequences.h \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OsyHexFile.h \
           ../libs/opensyde_core/scl/CSCLChecksums.h \
           ../libs/opensyde_core/scl/CSCLString.h \
           ../libs/opensyde_core/scl/CSCLStringList.h \
           ../libs/opensyde_core/scl/CSCLDateTime.h \
           ../libs/opensyde_core/scl/CSCLIniFile.h \
           ../libs/opensyde_core/xml_parser/C_OSCXMLParser.h \
           ../libs/opensyde_core/xml_parser/tinyxml2/tinyxml2.h \
           ../src/C_SYDEsup.h \
           ../src/C_SUPSuSequences.h

PRECOMPILED_HEADER = ../src/precomp_headers.h

#regular (non-library) header paths (also lists the system ones so QtCreator parser sees them)
INCLUDEPATH += ../src \
               ../libs/opensyde_core \
               ../libs/opensyde_core/can_dispatcher \
               ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls \
               ../libs/opensyde_core/can_dispatcher/dispatcher \
               ../libs/opensyde_core/ip_dispatcher/target_windows_win_sock \
               ../libs/opensyde_core/ip_dispatcher/dispatcher \
               ../libs/opensyde_core/data_dealer \
               ../libs/opensyde_core/data_dealer/paramset \
               ../libs/opensyde_core/exports \
               ../libs/opensyde_core/imports \
               ../libs/opensyde_core/logging \
               ../libs/opensyde_core/md5 \
               ../libs/opensyde_core/project \
               ../libs/opensyde_core/project/system \
               ../libs/opensyde_core/project/system/node \
               ../libs/opensyde_core/project/system/node/can \
               ../libs/opensyde_core/protocol_drivers \
               ../libs/opensyde_core/protocol_drivers/routing \
               ../libs/opensyde_core/protocol_drivers/communication \
               ../libs/opensyde_core/protocol_drivers/system_update \
               ../libs/opensyde_core/scl \
               ../libs/opensyde_core/stwtypes \
               ../libs/opensyde_core/kefex_diaglib \
               ../libs/opensyde_core/kefex_diaglib/tgl_windows \
               ../libs/opensyde_core/xml_parser \
               ../libs/opensyde_core/xml_parser/tinyxml2 \
               ../libs/opensyde_core/miniz \

DISTFILES += \
    ../doc/about.txt \
    ../doc/build.txt \
    ../doc/userlist.txt
