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

SOURCES += ../src/C_SUPSuSequences.cpp \
           ../src/C_SYDEsup.cpp \
           ../src/main.cpp \
           ../libs/opensyde_core/C_OSCUtils.cpp \
           ../libs/opensyde_core/C_OSCZipFile.cpp \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANBase.cpp \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANDispatcher.cpp \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCAN.cpp \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCANDLL.cpp \
           ../libs/opensyde_core/data_dealer/C_OSCDataDealer.cpp \
           ../libs/opensyde_core/data_dealer/C_OSCDataDealerNvm.cpp \
           ../libs/opensyde_core/data_dealer/C_OSCDataDealerNvmSafe.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetDataPoolInfo.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetFilerBase.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetHandler.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedData.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedDataPool.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedElement.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedFileInfoData.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedList.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedNode.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedNodeFiler.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetRawEntry.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetRawNode.cpp \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetRawNodeFiler.cpp \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfig.cpp \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigChannel.cpp \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigDomain.cpp \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigFiler.cpp \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigParameter.cpp \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigParameterStruct.cpp \
           ../libs/opensyde_core/halc/definition/base/C_OSCHalcDefBase.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDef.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefChannelAvailability.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefChannelUseCase.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefContent.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefContentBitmaskItem.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefDomain.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefElement.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefFiler.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefStruct.cpp \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefStructFiler.cpp \
           ../libs/opensyde_core/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.cpp \
           ../libs/opensyde_core/kefex_diaglib/CDLReportEvents.cpp \
           ../libs/opensyde_core/kefex_diaglib/CHexFile.cpp \
           ../libs/opensyde_core/kefex_diaglib/CSCLResourceStrings.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLActions.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLECUInformation.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWrite.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWriteParameters.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLHexFile.cpp \
           ../libs/opensyde_core/kefex_diaglib/CXFLProtocol.cpp \
           ../libs/opensyde_core/kefex_diaglib/DLStrings.cpp \
           ../libs/opensyde_core/kefex_diaglib/stwcompid.c \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLFile.cpp \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTasks.cpp \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTime.cpp \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLUtils.cpp \
           ../libs/opensyde_core/logging/C_OSCLoggingHandler.cpp \
           ../libs/opensyde_core/miniz/miniz.c \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinition.cpp \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinitionFiler.cpp \
           ../libs/opensyde_core/project/system/C_OSCDeviceGroup.cpp \
           ../libs/opensyde_core/project/system/C_OSCDeviceManager.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemBus.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemBusFiler.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinition.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinitionFiler.cpp \
           ../libs/opensyde_core/project/system/C_OSCSystemFilerUtil.cpp \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCNodeDataPoolFilerV2.cpp \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCNodeFilerV2.cpp \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCSystemBusFilerV2.cpp \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCSystemDefinitionFilerV2.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNode.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeApplication.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeComInterfaceSettings.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeCommFiler.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPool.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolContent.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolContentUtil.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolDataset.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolFiler.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolId.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolList.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListElement.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListElementId.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListId.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeFiler.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeOpenSydeServerSettings.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeProperties.cpp \
           ../libs/opensyde_core/project/system/node/C_OSCNodeStwFlashloaderSettings.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessage.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageContainer.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageIdentificationIndices.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanProtocol.cpp \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanSignal.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCCanDispatcherOsyRouter.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCDiagProtocolBase.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCDiagProtocolOsy.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCFlashProtocolStwFlashloader.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsy.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpBase.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpCan.cpp \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpIp.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverBase.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverFlash.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverProtocol.cpp \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComSequencesBase.cpp \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingCalculation.cpp \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingRoute.cpp \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuSequences.cpp \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.cpp \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OsyHexFile.cpp \
           ../libs/opensyde_core/scl/CSCLChecksums.cpp \
           ../libs/opensyde_core/scl/CSCLDateTime.cpp \
           ../libs/opensyde_core/scl/CSCLIniFile.cpp \
           ../libs/opensyde_core/scl/CSCLString.cpp \
           ../libs/opensyde_core/scl/CSCLStringList.cpp \
           ../libs/opensyde_core/xml_parser/C_OSCChecksummedXML.cpp \
           ../libs/opensyde_core/xml_parser/C_OSCXMLParser.cpp \
           ../libs/opensyde_core/xml_parser/tinyxml2/tinyxml2.cpp

HEADERS += ../src/C_SUPSuSequences.h \
           ../src/C_SYDEsup.h \
           ../libs/opensyde_core/C_OSCUtils.h \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANBase.h \
           ../libs/opensyde_core/can_dispatcher/dispatcher/CCANDispatcher.h \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCAN.h \
           ../libs/opensyde_core/can_dispatcher/target_windows_stw_dlls/CCANDLL.h \
           ../libs/opensyde_core/data_dealer/C_OSCDataDealer.h \
           ../libs/opensyde_core/data_dealer/C_OSCDataDealerNvm.h \
           ../libs/opensyde_core/data_dealer/C_OSCDataDealerNvmSafe.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetDataPoolInfo.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetFilerBase.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetHandler.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedData.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedDataPool.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedElement.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedFileInfoData.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedList.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedNode.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetInterpretedNodeFiler.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetRawEntry.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetRawNode.h \
           ../libs/opensyde_core/data_dealer/paramset/C_OSCParamSetRawNodeFiler.h \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfig.h \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigChannel.h \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigDomain.h \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigFiler.h \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigParameter.h \
           ../libs/opensyde_core/halc/configuration/C_OSCHalcConfigParameterStruct.h \
           ../libs/opensyde_core/halc/definition/base/C_OSCHalcDefBase.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDef.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefChannelAvailability.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefChannelUseCase.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefContent.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefContentBitmaskItem.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefDomain.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefElement.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefFiler.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefStruct.h \
           ../libs/opensyde_core/halc/definition/C_OSCHalcDefStructFiler.h \
           ../libs/opensyde_core/ip_dispatcher/dispatcher/C_OSCIpDispatcher.h \
           ../libs/opensyde_core/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.h \
           ../libs/opensyde_core/kefex_diaglib/CDLReportEvents.h \
           ../libs/opensyde_core/kefex_diaglib/CHexFile.h \
           ../libs/opensyde_core/kefex_diaglib/CSCLResourceStrings.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLActions.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLECUInformation.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWrite.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLFlashWriteParameters.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLHexFile.h \
           ../libs/opensyde_core/kefex_diaglib/CXFLProtocol.h \
           ../libs/opensyde_core/kefex_diaglib/DLStrings.h \
           ../libs/opensyde_core/kefex_diaglib/stwcompid.c \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLFile.h \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTasks.h \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLTime.h \
           ../libs/opensyde_core/kefex_diaglib/tgl_windows/TGLUtils.h \
           ../libs/opensyde_core/logging/C_OSCLoggingHandler.h \
           ../libs/opensyde_core/miniz/miniz.c \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinition.h \
           ../libs/opensyde_core/project/system/C_OSCDeviceDefinitionFiler.h \
           ../libs/opensyde_core/project/system/C_OSCDeviceGroup.h \
           ../libs/opensyde_core/project/system/C_OSCDeviceManager.h \
           ../libs/opensyde_core/project/system/C_OSCSystemBus.h \
           ../libs/opensyde_core/project/system/C_OSCSystemBusFiler.h \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinition.h \
           ../libs/opensyde_core/project/system/C_OSCSystemDefinitionFiler.h \
           ../libs/opensyde_core/project/system/C_OSCSystemFilerUtil.h \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCNodeDataPoolFilerV2.h \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCNodeFilerV2.h \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCSystemBusFilerV2.h \
           ../libs/opensyde_core/project/system/FileLoadersV2/C_OSCSystemDefinitionFilerV2.h \
           ../libs/opensyde_core/project/system/node/C_OSCNode.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeApplication.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeComInterfaceSettings.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeCommFiler.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPool.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolContent.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolContentUtil.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolDataset.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolExportSettings.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolFiler.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolId.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolList.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListElement.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListElementId.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeDataPoolListId.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeFiler.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeOpenSydeServerSettings.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeProperties.h \
           ../libs/opensyde_core/project/system/node/C_OSCNodeStwFlashloaderSettings.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessage.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageContainer.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanMessageIdentificationIndices.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanProtocol.h \
           ../libs/opensyde_core/project/system/node/can/C_OSCCanSignal.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCCanDispatcherOsyRouter.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCDiagProtocolBase.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCDiagProtocolOsy.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCFlashProtocolStwFlashloader.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsy.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpBase.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpCan.h \
           ../libs/opensyde_core/protocol_drivers/C_OSCProtocolDriverOsyTpIp.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverBase.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverFlash.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComDriverProtocol.h \
           ../libs/opensyde_core/protocol_drivers/communication/C_OSCComSequencesBase.h \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingCalculation.h \
           ../libs/opensyde_core/protocol_drivers/routing/C_OSCRoutingRoute.h \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuSequences.h \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.h \
           ../libs/opensyde_core/protocol_drivers/system_update/C_OsyHexFile.h \
           ../libs/opensyde_core/scl/CSCLChecksums.h \
           ../libs/opensyde_core/scl/CSCLDateTime.h \
           ../libs/opensyde_core/scl/CSCLIniFile.h \
           ../libs/opensyde_core/scl/CSCLString.h \
           ../libs/opensyde_core/scl/CSCLString.h \
           ../libs/opensyde_core/scl/CSCLStringList.h \
           ../libs/opensyde_core/xml_parser/C_OSCChecksummedXML.h \
           ../libs/opensyde_core/xml_parser/C_OSCXMLParser.h \
           ../libs/opensyde_core/xml_parser/tinyxml2/tinyxml2.h

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
               ../libs/opensyde_core/halc \
               ../libs/opensyde_core/halc/configuration \
               ../libs/opensyde_core/halc/definition \
               ../libs/opensyde_core/halc/definition/base \
               ../libs/opensyde_core/logging \
               ../libs/opensyde_core/project \
               ../libs/opensyde_core/project/system \
               ../libs/opensyde_core/project/system/node \
               ../libs/opensyde_core/project/system/node/can \
               ../libs/opensyde_core/project/system/FileLoadersV2 \
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
