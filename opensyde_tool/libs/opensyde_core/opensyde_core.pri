# This file lists all openSYDE Core files and include paths in qmake syntax.
# It can be included from a qmake .pro file of an application using the openSYDE Core.
# Some optional parts of the Core can be skipped by setting options in the .pro file before including this file
#  in order to keep build times low. e.g.:
#
# opensyde_core_skip_modules += opensyde_core_skip_code_generation
# include(../libs/osy_core/opensyde_core.pri)
#
# See the "opensyde_core_skip_*" checks below for parts that can be skipped

#shout out aloud so we have a clearly distinguishable headline in the log:
message("opensyde_core.pri: --------------------- ADDING PACKAGES ---------------------")

# basics:
INCLUDEPATH += $${PWD} \
               $${PWD}/kefex_diaglib \
               $${PWD}/kefex_diaglib/tgl_windows \
               $${PWD}/logging \
               $${PWD}/md5 \
               $${PWD}/scl \
               $${PWD}/stwtypes \
               $${PWD}/xml_parser \
               $${PWD}/xml_parser/tinyxml2

SOURCES += \
    $${PWD}/C_OSCBinaryHash.cpp \
    $${PWD}/C_OSCChecksummedIniFile.cpp \
    $${PWD}/C_OSCUtils.cpp \
    $${PWD}/kefex_diaglib/CSCLResourceStrings.cpp \
    $${PWD}/kefex_diaglib/DLStrings.cpp \
    $${PWD}/kefex_diaglib/stwcompid.c \
    $${PWD}/kefex_diaglib/tgl_windows/TGLFile.cpp \
    $${PWD}/kefex_diaglib/tgl_windows/TGLTasks.cpp \
    $${PWD}/kefex_diaglib/tgl_windows/TGLTime.cpp \
    $${PWD}/kefex_diaglib/tgl_windows/TGLUtils.cpp \
    $${PWD}/logging/C_OSCLoggingHandler.cpp \
    $${PWD}/md5/CMD5Checksum.cpp \
    $${PWD}/scl/CSCLChecksums.cpp \
    $${PWD}/scl/CSCLDateTime.cpp \
    $${PWD}/scl/CSCLIniFile.cpp \
    $${PWD}/scl/CSCLString.cpp \
    $${PWD}/scl/CSCLStringList.cpp \
    $${PWD}/xml_parser/C_OSCXMLParser.cpp \
    $${PWD}/xml_parser/C_OSCXMLParserLog.cpp \
    $${PWD}/xml_parser/C_OSCChecksummedXML.cpp \
    $${PWD}/xml_parser/tinyxml2/tinyxml2.cpp

HEADERS += \
    $${PWD}/C_OSCBinaryHash.h \
    $${PWD}/C_OSCChecksummedIniFile.h \
    $${PWD}/C_OSCUtils.h \
    $${PWD}/kefex_diaglib/CSCLResourceStrings.h \
    $${PWD}/kefex_diaglib/DLStrings.h \
    $${PWD}/kefex_diaglib/stwcompid.h \
    $${PWD}/kefex_diaglib/tgl_windows/TGLFile.h \
    $${PWD}/kefex_diaglib/tgl_windows/TGLTasks.h \
    $${PWD}/kefex_diaglib/tgl_windows/TGLTime.h \
    $${PWD}/kefex_diaglib/tgl_windows/TGLUtils.h \
    $${PWD}/logging/C_OSCLoggingHandler.h \
    $${PWD}/md5/CMD5Checksum.h \
    $${PWD}/scl/CSCLChecksums.h \
    $${PWD}/scl/CSCLDateTime.h \
    $${PWD}/scl/CSCLIniFile.h \
    $${PWD}/scl/CSCLString.h \
    $${PWD}/scl/CSCLStringList.h \
    $${PWD}/scl/SCLDynamicArray.h \
    $${PWD}/stwerrors.h \
    $${PWD}/stwtypes/stwtypes.h \
    $${PWD}/xml_parser/C_OSCXMLParser.h \
    $${PWD}/xml_parser/C_OSCXMLParserLog.h \
    $${PWD}/xml_parser/C_OSCChecksummedXML.h \
    $${PWD}/xml_parser/tinyxml2/tinyxml2.h

# optional: zip/unzip
contains(opensyde_core_skip_modules, opensyde_core_skip_zipping) {
   message("opensyde_core_skip_zipping detected ... skipping package")
} else {
   message("opensyde_core_skip_zipping not detected ... dragging in package")

   INCLUDEPATH += \
               $${PWD}/aes \
               $${PWD}/miniz

   SOURCES += \
    $${PWD}/C_OSCAesFile.cpp \
    $${PWD}/C_OSCZipData.cpp \
    $${PWD}/C_OSCZipFile.cpp \
    $${PWD}/aes/AES.cpp \
    $${PWD}/miniz/miniz.c

   HEADERS += \
    $${PWD}/C_OSCAesFile.h \
    $${PWD}/C_OSCZipData.h \
    $${PWD}/C_OSCZipFile.h \
    $${PWD}/aes/AES.h \
    $${PWD}/miniz/miniz.h
}

# optional: imports
contains(opensyde_core_skip_modules, opensyde_core_skip_imports) {
   message("opensyde_core_skip_imports detected ... skipping package")
} else {
   message("opensyde_core_skip_imports not detected ... dragging in package")

   INCLUDEPATH += \
               $${PWD}/imports

   SOURCES += \
    $${PWD}/imports/C_OSCCanOpenObjectDictionary.cpp \
    $${PWD}/imports/C_OSCImportEdsDcf.cpp

   HEADERS += \
    $${PWD}/imports/C_OSCCanOpenObjectDictionary.h \
    $${PWD}/imports/C_OSCImportEdsDcf.h
}

# optional: file generation
contains(opensyde_core_skip_modules, opensyde_core_skip_code_generation) {
   message("opensyde_core_skip_code_generation detected ... skipping package")
} else {
   message("opensyde_core_skip_code_generation not detected ... dragging in package")

   INCLUDEPATH += \
               $${PWD}/exports

   SOURCES += \
    $${PWD}/exports/C_OSCExportCommunicationStack.cpp \
    $${PWD}/exports/C_OSCExportDataPool.cpp \
    $${PWD}/exports/C_OSCExportHalc.cpp \
    $${PWD}/exports/C_OSCExportNode.cpp \
    $${PWD}/exports/C_OSCExportOsyInit.cpp \
    $${PWD}/exports/C_OSCExportParamSet.cpp \
    $${PWD}/exports/C_OSCExportUti.cpp

   HEADERS += \
    $${PWD}/exports/C_OSCExportCommunicationStack.h \
    $${PWD}/exports/C_OSCExportDataPool.h \
    $${PWD}/exports/C_OSCExportHalc.h \
    $${PWD}/exports/C_OSCExportNode.h \
    $${PWD}/exports/C_OSCExportOsyInit.h \
    $${PWD}/exports/C_OSCExportParamSet.h \
    $${PWD}/exports/C_OSCExportUti.h
}

# optional: project handling (Including all aspects of System Definition)
contains(opensyde_core_skip_modules, opensyde_core_skip_project_handling) {
   message("opensyde_core_skip_project_handling detected ... skipping package")
} else {
   message("opensyde_core_skip_project_handling not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/halc \
                  $${PWD}/halc/configuration \
                  $${PWD}/halc/configuration/standalone \
                  $${PWD}/halc/definition \
                  $${PWD}/halc/definition/base \
                  $${PWD}/halc/magician \
                  $${PWD}/project \
                  $${PWD}/project/system \
                  $${PWD}/project/system/FileLoadersV2 \
                  $${PWD}/project/system/node \
                  $${PWD}/project/system/node/can

   SOURCES += \
       $${PWD}/halc/configuration/C_OSCHalcConfig.cpp \
       $${PWD}/halc/configuration/C_OSCHalcConfigChannel.cpp \
       $${PWD}/halc/configuration/C_OSCHalcConfigDomain.cpp \
       $${PWD}/halc/configuration/C_OSCHalcConfigFiler.cpp \
       $${PWD}/halc/configuration/C_OSCHalcConfigParameter.cpp \
       $${PWD}/halc/configuration/C_OSCHalcConfigParameterStruct.cpp \
       $${PWD}/halc/configuration/C_OSCHalcConfigUtil.cpp \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandalone.cpp \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandaloneChannel.cpp \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandaloneDomain.cpp \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandaloneFiler.cpp \
       $${PWD}/halc/definition/base/C_OSCHalcDefBase.cpp \
       $${PWD}/halc/definition/C_OSCHalcDef.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefChannelAvailability.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefChannelDef.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefChannelUseCase.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefChannelValues.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefContent.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefContentBitmaskItem.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefDomain.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefElement.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefFiler.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefStruct.cpp \
       $${PWD}/halc/definition/C_OSCHalcDefStructFiler.cpp \
       $${PWD}/halc/magician/C_OSCHALCMagicianDatapoolListHandler.cpp \
       $${PWD}/halc/magician/C_OSCHALCMagicianGenerator.cpp \
       $${PWD}/halc/magician/C_OSCHALCMagicianUtil.cpp \
       $${PWD}/project/C_OSCProject.cpp \
       $${PWD}/project/C_OSCProjectFiler.cpp \
       $${PWD}/project/system/C_OSCDeviceDefinitionFiler.cpp \
       $${PWD}/project/system/C_OSCDeviceDefinitionFilerV1.cpp \
       $${PWD}/project/system/C_OSCDeviceDefinition.cpp \
       $${PWD}/project/system/C_OSCSubDeviceDefinition.cpp \
       $${PWD}/project/system/C_OSCDeviceGroup.cpp \
       $${PWD}/project/system/C_OSCDeviceManager.cpp \
       $${PWD}/project/system/C_OSCSystemBus.cpp \
       $${PWD}/project/system/C_OSCSystemBusFiler.cpp \
       $${PWD}/project/system/C_OSCSystemDefinition.cpp \
       $${PWD}/project/system/C_OSCSystemDefinitionFiler.cpp \
       $${PWD}/project/system/C_OSCSystemFilerUtil.cpp \
       $${PWD}/project/system/C_OSCTargetSupportPackage.cpp \
       $${PWD}/project/system/C_OSCTargetSupportPackageFiler.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OSCNodeDataPoolFilerV2.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OSCNodeFilerV2.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OSCSystemBusFilerV2.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OSCSystemDefinitionFilerV2.cpp \
       $${PWD}/project/system/node/can/C_OSCCanMessage.cpp \
       $${PWD}/project/system/node/can/C_OSCCanMessageUniqueId.cpp \
       $${PWD}/project/system/node/can/C_OSCCanMessageContainer.cpp \
       $${PWD}/project/system/node/can/C_OSCCanMessageIdentificationIndices.cpp \
       $${PWD}/project/system/node/can/C_OSCCanProtocol.cpp \
       $${PWD}/project/system/node/can/C_OSCCanSignal.cpp \
       $${PWD}/project/system/node/can/C_OSCCanUtil.cpp \
       $${PWD}/project/system/node/C_OSCNode.cpp \
       $${PWD}/project/system/node/C_OSCNodeSquad.cpp \
       $${PWD}/project/system/node/C_OSCNodeSquadFiler.cpp \
       $${PWD}/project/system/node/C_OSCNodeApplication.cpp \
       $${PWD}/project/system/node/C_OSCNodeCodeExportSettings.cpp \
       $${PWD}/project/system/node/C_OSCNodeComInterfaceSettings.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPool.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolContent.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolContentUtil.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolDataSet.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolFiler.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolList.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolListElement.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolListElementId.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolListId.cpp \
       $${PWD}/project/system/node/C_OSCNodeFiler.cpp \
       $${PWD}/project/system/node/C_OSCNodeOpenSydeServerSettings.cpp \
       $${PWD}/project/system/node/C_OSCNodeProperties.cpp \
       $${PWD}/project/system/node/C_OSCNodeStwFlashloaderSettings.cpp \
       $${PWD}/project/system/node/C_OSCNodeCommFiler.cpp \
       $${PWD}/project/system/node/C_OSCNodeDataPoolId.cpp

   HEADERS += \
       $${PWD}/halc/configuration/C_OSCHalcConfig.h \
       $${PWD}/halc/configuration/C_OSCHalcConfigChannel.h \
       $${PWD}/halc/configuration/C_OSCHalcConfigDomain.h \
       $${PWD}/halc/configuration/C_OSCHalcConfigFiler.h \
       $${PWD}/halc/configuration/C_OSCHalcConfigParameter.h \
       $${PWD}/halc/configuration/C_OSCHalcConfigParameterStruct.h \
       $${PWD}/halc/configuration/C_OSCHalcConfigUtil.h \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandalone.h \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandaloneChannel.h \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandaloneDomain.h \
       $${PWD}/halc/configuration/standalone/C_OSCHalcConfigStandaloneFiler.h \
       $${PWD}/halc/definition/base/C_OSCHalcDefBase.h \
       $${PWD}/halc/definition/C_OSCHalcDef.h \
       $${PWD}/halc/definition/C_OSCHalcDefChannelAvailability.h \
       $${PWD}/halc/definition/C_OSCHalcDefChannelDef.h \
       $${PWD}/halc/definition/C_OSCHalcDefChannelUseCase.h \
       $${PWD}/halc/definition/C_OSCHalcDefChannelValues.h \
       $${PWD}/halc/definition/C_OSCHalcDefContent.h \
       $${PWD}/halc/definition/C_OSCHalcDefContentBitmaskItem.h \
       $${PWD}/halc/definition/C_OSCHalcDefDomain.h \
       $${PWD}/halc/definition/C_OSCHalcDefElement.h \
       $${PWD}/halc/definition/C_OSCHalcDefFiler.h \
       $${PWD}/halc/definition/C_OSCHalcDefStruct.h \
       $${PWD}/halc/definition/C_OSCHalcDefStructFiler.h \
       $${PWD}/halc/magician/C_OSCHALCMagicianDatapoolListHandler.h \
       $${PWD}/halc/magician/C_OSCHALCMagicianGenerator.h \
       $${PWD}/halc/magician/C_OSCHALCMagicianUtil.h \
       $${PWD}/project/C_OSCProject.h \
       $${PWD}/project/C_OSCProjectFiler.h \
       $${PWD}/project/system/C_OSCDeviceDefinition.h \
       $${PWD}/project/system/C_OSCDeviceDefinitionFiler.h \
       $${PWD}/project/system/C_OSCDeviceDefinitionFilerV1.h \
       $${PWD}/project/system/C_OSCSubDeviceDefinition.h \
       $${PWD}/project/system/C_OSCDeviceGroup.h \
       $${PWD}/project/system/C_OSCDeviceManager.h \
       $${PWD}/project/system/C_OSCSystemBus.h \
       $${PWD}/project/system/C_OSCSystemBusFiler.h \
       $${PWD}/project/system/C_OSCSystemDefinition.h \
       $${PWD}/project/system/C_OSCSystemDefinitionFiler.h \
       $${PWD}/project/system/C_OSCSystemFilerUtil.h \
       $${PWD}/project/system/C_OSCTargetSupportPackage.h \
       $${PWD}/project/system/C_OSCTargetSupportPackageFiler.h \
       $${PWD}/project/system/FileLoadersV2/C_OSCNodeDataPoolFilerV2.h \
       $${PWD}/project/system/FileLoadersV2/C_OSCNodeFilerV2.h \
       $${PWD}/project/system/FileLoadersV2/C_OSCSystemBusFilerV2.h \
       $${PWD}/project/system/FileLoadersV2/C_OSCSystemDefinitionFilerV2.h \
       $${PWD}/project/system/node/C_OSCNode.h \
       $${PWD}/project/system/node/C_OSCNodeSquad.h \
       $${PWD}/project/system/node/C_OSCNodeSquadFiler.h \
       $${PWD}/project/system/node/C_OSCNodeApplication.h \
       $${PWD}/project/system/node/C_OSCNodeCodeExportSettings.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolContentUtil.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolId.h \
       $${PWD}/project/system/node/C_OSCNodeComInterfaceSettings.h \
       $${PWD}/project/system/node/C_OSCNodeCommFiler.h \
       $${PWD}/project/system/node/C_OSCNodeDataPool.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolContent.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolDataSet.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolFiler.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolList.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolListElement.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolListElementId.h \
       $${PWD}/project/system/node/C_OSCNodeDataPoolListId.h \
       $${PWD}/project/system/node/C_OSCNodeFiler.h \
       $${PWD}/project/system/node/C_OSCNodeOpenSydeServerSettings.h \
       $${PWD}/project/system/node/C_OSCNodeProperties.h \
       $${PWD}/project/system/node/C_OSCNodeStwFlashloaderSettings.h \
       $${PWD}/project/system/node/can/C_OSCCanMessage.h \
       $${PWD}/project/system/node/can/C_OSCCanMessageUniqueId.h \
       $${PWD}/project/system/node/can/C_OSCCanMessageContainer.h \
       $${PWD}/project/system/node/can/C_OSCCanMessageIdentificationIndices.h \
       $${PWD}/project/system/node/can/C_OSCCanProtocol.h \
       $${PWD}/project/system/node/can/C_OSCCanSignal.h \
       $${PWD}/project/system/node/can/C_OSCCanUtil.h
}

# optional: parameter set handling (reading and writing syde_psi files)
contains(opensyde_core_skip_modules, opensyde_core_skip_param_set) {
   message("opensyde_core_skip_param_set detected ... skipping package")
} else {
   message("opensyde_core_skip_param_set not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/data_dealer/paramset

   SOURCES += $${PWD}/data_dealer/paramset/C_OSCParamSetDataPoolInfo.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetFilerBase.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetHandler.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedData.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedDataPool.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedElement.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedFileInfoData.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedList.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedNode.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedNodeFiler.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetRawEntry.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetRawNode.cpp \
              $${PWD}/data_dealer/paramset/C_OSCParamSetRawNodeFiler.cpp

   HEADERS += $${PWD}/data_dealer/paramset/C_OSCParamSetDataPoolInfo.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetFilerBase.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetHandler.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedData.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedDataPool.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedElement.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedFileInfoData.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedList.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedNode.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetInterpretedNodeFiler.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetRawEntry.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetRawNode.h \
              $${PWD}/data_dealer/paramset/C_OSCParamSetRawNodeFiler.h
}

# optional: CAN and Ethernet dispatcher interface and Windows implementation
contains(opensyde_core_skip_modules, opensyde_core_skip_com_dispatchers) {
   message("opensyde_core_skip_com_dispatchers detected ... skipping package")
} else {
   message("opensyde_core_skip_com_dispatchers not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/can_dispatcher/dispatcher \
                  $${PWD}/can_dispatcher/target_windows_stw_dlls \
                  $${PWD}/ip_dispatcher/dispatcher \
                  $${PWD}/ip_dispatcher/target_windows_win_sock

   SOURCES += $${PWD}/can_dispatcher/dispatcher/CCANBase.cpp \
            $${PWD}/can_dispatcher/dispatcher/CCANDispatcher.cpp \
            $${PWD}/can_dispatcher/target_windows_stw_dlls/CCAN.cpp \
            $${PWD}/can_dispatcher/target_windows_stw_dlls/CCANDLL.cpp \
            $${PWD}/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.cpp

   HEADERS += $${PWD}/can_dispatcher/dispatcher/stw_can.h \
              $${PWD}/can_dispatcher/dispatcher/CCANBase.h \
              $${PWD}/can_dispatcher/dispatcher/CCANDispatcher.h \
              $${PWD}/can_dispatcher/target_windows_stw_dlls/CCAN.h \
              $${PWD}/can_dispatcher/target_windows_stw_dlls/CCANDLL.h \
              $${PWD}/ip_dispatcher/dispatcher/C_OSCIpDispatcher.h \
              $${PWD}/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.h
}


# optional: protocol drivers (openSYDE, STW Flashloader, KEFEX, Data Dealer)
# Also requires CAN and Ethernet dispatchers
contains(opensyde_core_skip_modules, opensyde_core_skip_protocol_drivers) {
   message("opensyde_core_skip_protocol_drivers detected ... skipping package")
} else {
   message("opensyde_core_skip_protocol_drivers not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/data_dealer \
                  $${PWD}/security \
                  $${PWD}/protocol_drivers \
                  $${PWD}/protocol_drivers/routing \
                  $${PWD}/protocol_drivers/communication \
                  $${PWD}/protocol_drivers/system_update \
                  $${PWD}/kefex_diaglib

   SOURCES += $${PWD}/data_dealer/C_OSCDataDealer.cpp \
              $${PWD}/data_dealer/C_OSCDataDealerNvm.cpp \
              $${PWD}/data_dealer/C_OSCDataDealerNvmSafe.cpp \
              $${PWD}/kefex_diaglib/CDLReportEvents.cpp \
              $${PWD}/kefex_diaglib/CHexFile.cpp \
              $${PWD}/kefex_diaglib/CKFXComm.cpp \
              $${PWD}/kefex_diaglib/CKFXCommConfiguration.cpp \
              $${PWD}/kefex_diaglib/CKFXCommunicationKEFEX.cpp \
              $${PWD}/kefex_diaglib/CKFXProtocol.cpp \
              $${PWD}/kefex_diaglib/CXFLActions.cpp \
              $${PWD}/kefex_diaglib/CXFLECUInformation.cpp \
              $${PWD}/kefex_diaglib/CXFLFlashWrite.cpp \
              $${PWD}/kefex_diaglib/CXFLFlashWriteParameters.cpp \
              $${PWD}/kefex_diaglib/CXFLHexFile.cpp \
              $${PWD}/kefex_diaglib/CXFLProtocol.cpp \
              $${PWD}/security/C_OSCSecurityPem.cpp \
              $${PWD}/security/C_OSCSecurityPemKeyInfo.cpp \
              $${PWD}/security/C_OSCSecurityPemDatabase.cpp \
              $${PWD}/security/C_OSCSecurityRsa.cpp \
              $${PWD}/protocol_drivers/C_OSCCanDispatcherOsyRouter.cpp \
              $${PWD}/protocol_drivers/C_OSCDiagProtocolBase.cpp \
              $${PWD}/protocol_drivers/C_OSCDiagProtocolKfx.cpp \
              $${PWD}/protocol_drivers/C_OSCDiagProtocolOsy.cpp \
              $${PWD}/protocol_drivers/C_OSCFlashProtocolStwFlashloader.cpp \
              $${PWD}/protocol_drivers/C_OSCProtocolSerialNumber.cpp \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsy.cpp \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsyTpBase.cpp \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsyTpCan.cpp \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsyTpIp.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComDriverBase.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComDriverFlash.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComDriverProtocol.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComSequencesBase.cpp \
              $${PWD}/protocol_drivers/routing/C_OSCRoutingCalculation.cpp \
              $${PWD}/protocol_drivers/routing/C_OSCRoutingRoute.cpp \
              $${PWD}/protocol_drivers/system_update/C_OSCSuSequences.cpp \
              $${PWD}/protocol_drivers/system_update/C_OSCSuSequencesNodeStates.cpp \
              $${PWD}/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.cpp \
              $${PWD}/protocol_drivers/system_update/C_OsyHexFile.cpp

   HEADERS += $${PWD}/data_dealer/C_OSCDataDealer.h \
              $${PWD}/data_dealer/C_OSCDataDealerNvm.h \
              $${PWD}/data_dealer/C_OSCDataDealerNvmSafe.h \
              $${PWD}/kefex_diaglib/CDLReportEvents.h \
              $${PWD}/kefex_diaglib/CHexFile.h \
              $${PWD}/kefex_diaglib/CKFXComm.h \
              $${PWD}/kefex_diaglib/CKFXCommConfiguration.h \
              $${PWD}/kefex_diaglib/CKFXCommunicationKEFEX.h \
              $${PWD}/kefex_diaglib/CKFXProtocol.h \
              $${PWD}/kefex_diaglib/CXFLActions.h \
              $${PWD}/kefex_diaglib/CXFLECUInformation.h \
              $${PWD}/kefex_diaglib/CXFLFlashWrite.h \
              $${PWD}/kefex_diaglib/CXFLFlashWriteParameters.h \
              $${PWD}/kefex_diaglib/CXFLHexFile.h \
              $${PWD}/kefex_diaglib/CXFLProtocol.h \
              $${PWD}/security/C_OSCSecurityPem.h \
              $${PWD}/security/C_OSCSecurityPemKeyInfo.h \
              $${PWD}/security/C_OSCSecurityPemDatabase.h \
              $${PWD}/security/C_OSCSecurityRsa.h \
              $${PWD}/protocol_drivers/C_OSCCanDispatcherOsyRouter.h \
              $${PWD}/protocol_drivers/C_OSCDiagProtocolBase.h \
              $${PWD}/protocol_drivers/C_OSCDiagProtocolKfx.h \
              $${PWD}/protocol_drivers/C_OSCDiagProtocolOsy.h \
              $${PWD}/protocol_drivers/C_OSCFlashProtocolStwFlashloader.h \
              $${PWD}/protocol_drivers/C_OSCProtocolSerialNumber.h \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsy.h \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsyTpBase.h \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsyTpCan.h \
              $${PWD}/protocol_drivers/C_OSCProtocolDriverOsyTpIp.h \
              $${PWD}/protocol_drivers/communication/C_OSCComDriverBase.h \
              $${PWD}/protocol_drivers/communication/C_OSCComDriverFlash.h \
              $${PWD}/protocol_drivers/communication/C_OSCComDriverProtocol.h \
              $${PWD}/protocol_drivers/communication/C_OSCComSequencesBase.h \
              $${PWD}/protocol_drivers/routing/C_OSCRoutingCalculation.h \
              $${PWD}/protocol_drivers/routing/C_OSCRoutingRoute.h \
              $${PWD}/protocol_drivers/system_update/C_OSCSuSequences.h \
              $${PWD}/protocol_drivers/system_update/C_OSCSuSequencesNodeStates.h \
              $${PWD}/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.h \
              $${PWD}/protocol_drivers/system_update/C_OsyHexFile.h \
              $${PWD}/protocol_drivers/DiagLib_config.h

   include(./libtomcrypt/libtomcrypt.pri)
}

# optional: protocol logging
# Also requires CAN and Ethernet dispatchers
# Note that C_OSCComDriverBase.cpp is part of this package and of the protocol drivers as it's needed in both cases
contains(opensyde_core_skip_modules, opensyde_core_skip_protocol_logging) {
   message("opensyde_core_skip_protocol_logging detected ... skipping package")
} else {
   message("opensyde_core_skip_protocol_logging not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/kefex_diaglib \
                  $${PWD}/protocol_drivers/communication

   SOURCES += $${PWD}/kefex_diaglib/CCMONProtocol.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolBase.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolCANopen.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolGD.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolJ1939.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolKEFEX.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolL2.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolOpenSYDE.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolSHIPIPIVA.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolSTWFF.cpp \
              $${PWD}/kefex_diaglib/CCMONProtocolXFL.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComLoggerProtocolOpenSYDE.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComLoggerProtocols.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLogger.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerData.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerFileAsc.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerFileBase.cpp \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerOsySysDefConfig.cpp

   HEADERS += $${PWD}/kefex_diaglib/CCMONProtocol.h \
              $${PWD}/kefex_diaglib/CCMONProtocolBase.h \
              $${PWD}/kefex_diaglib/CCMONProtocolCANopen.h \
              $${PWD}/kefex_diaglib/CCMONProtocolGD.h \
              $${PWD}/kefex_diaglib/CCMONProtocolJ1939.h \
              $${PWD}/kefex_diaglib/CCMONProtocolKEFEX.h \
              $${PWD}/kefex_diaglib/CCMONProtocolL2.h \
              $${PWD}/kefex_diaglib/CCMONProtocolOpenSYDE.h \
              $${PWD}/kefex_diaglib/CCMONProtocolSHIPIPIVA.h \
              $${PWD}/kefex_diaglib/CCMONProtocolSTWFF.h \
              $${PWD}/kefex_diaglib/CCMONProtocolTarget.h \
              $${PWD}/kefex_diaglib/CCMONProtocolXFL.h \
              $${PWD}/kefex_diaglib/CDLReportEvents.h \
              $${PWD}/kefex_diaglib/CDLReportEvents.h \
              $${PWD}/protocol_drivers/communication/C_OSCComLoggerProtocolOpenSYDE.h \
              $${PWD}/protocol_drivers/communication/C_OSCComLoggerProtocols.h \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLogger.h \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerData.h \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerFileAsc.h \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerFileBase.h \
              $${PWD}/protocol_drivers/communication/C_OSCComMessageLoggerOsySysDefConfig.h

   ! contains(SOURCES, $${PWD}/protocol_drivers/communication/C_OSCComDriverBase.cpp) {
      SOURCES += $${PWD}/protocol_drivers/communication/C_OSCComDriverBase.cpp
      HEADERS += $${PWD}/protocol_drivers/communication/C_OSCComDriverBase.h
   }

}
