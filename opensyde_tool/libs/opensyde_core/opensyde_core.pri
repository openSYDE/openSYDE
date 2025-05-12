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
               $${PWD}/kefex_diaglib/dl_string_resources \
               $${PWD}/kefex_diaglib/stwcompid \
               $${PWD}/kefex_diaglib/tgl_windows \
               $${PWD}/logging \
               $${PWD}/md5 \
               $${PWD}/scl \
               $${PWD}/stwtypes \
               $${PWD}/xml_parser \
               $${PWD}/xml_parser/tinyxml2

SOURCES += \
    $$PWD/protocol_drivers/communication/C_OscComAutoSupport.cpp \
    $${PWD}/C_OscBinaryHash.cpp \
    $${PWD}/C_OscChecksummedIniFile.cpp \
    $${PWD}/C_OscUtils.cpp \
    $${PWD}/kefex_diaglib/dl_string_resources/C_SclResourceStrings.cpp \
    $${PWD}/kefex_diaglib/dl_string_resources/DLStrings.cpp \
    $${PWD}/kefex_diaglib/stwcompid/stwcompid.c \
    $${PWD}/kefex_diaglib/tgl_windows/TglFile.cpp \
    $${PWD}/kefex_diaglib/tgl_windows/TglTasks.cpp \
    $${PWD}/kefex_diaglib/tgl_windows/TglTime.cpp \
    $${PWD}/kefex_diaglib/tgl_windows/TglUtils.cpp \
    $${PWD}/logging/C_OscLoggingHandler.cpp \
    $${PWD}/md5/C_Md5Checksum.cpp \
    $${PWD}/scl/C_SclChecksums.cpp \
    $${PWD}/scl/C_SclDateTime.cpp \
    $${PWD}/scl/C_SclIniFile.cpp \
    $${PWD}/scl/C_SclString.cpp \
    $${PWD}/scl/C_SclStringList.cpp \
    $${PWD}/xml_parser/C_OscXmlParser.cpp \
    $${PWD}/xml_parser/C_OscXmlParserLog.cpp \
    $${PWD}/xml_parser/C_OscChecksummedXml.cpp \
    $${PWD}/xml_parser/tinyxml2/tinyxml2.cpp

HEADERS += \
    $$PWD/protocol_drivers/communication/C_OscComAutoSupport.hpp \
    $${PWD}/C_OscBinaryHash.hpp \
    $${PWD}/C_OscChecksummedIniFile.hpp \
    $${PWD}/C_OscUtils.hpp \
    $${PWD}/DiagLib_config.hpp \
    $${PWD}/kefex_diaglib/dl_string_resources/C_SclResourceStrings.hpp \
    $${PWD}/kefex_diaglib/dl_string_resources/DLStrings.hpp \
    $${PWD}/kefex_diaglib/stwcompid/stwcompid.h \
    $${PWD}/kefex_diaglib/tgl_windows/TglFile.hpp \
    $${PWD}/kefex_diaglib/tgl_windows/TglTasks.hpp \
    $${PWD}/kefex_diaglib/tgl_windows/TglTime.hpp \
    $${PWD}/kefex_diaglib/tgl_windows/TglUtils.hpp \
    $${PWD}/logging/C_OscLoggingHandler.hpp \
    $${PWD}/md5/C_Md5Checksum.hpp \
    $${PWD}/scl/C_SclChecksums.hpp \
    $${PWD}/scl/C_SclDateTime.hpp \
    $${PWD}/scl/C_SclIniFile.hpp \
    $${PWD}/scl/C_SclString.hpp \
    $${PWD}/scl/C_SclStringList.hpp \
    $${PWD}/scl/C_SclDynamicArray.hpp \
    $${PWD}/stwerrors.h \
    $${PWD}/stwerrors.hpp \
    $${PWD}/stwtypes/stwtypes.h \
    $${PWD}/stwtypes/stwtypes.hpp \
    $${PWD}/xml_parser/C_OscXmlParser.hpp \
    $${PWD}/xml_parser/C_OscXmlParserLog.hpp \
    $${PWD}/xml_parser/C_OscChecksummedXml.hpp \
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
    $${PWD}/C_OscAesFile.cpp \
    $${PWD}/C_OscZipData.cpp \
    $${PWD}/C_OscZipFile.cpp \
    $${PWD}/aes/AES.cpp \
    $${PWD}/miniz/miniz.c

   HEADERS += \
    $${PWD}/C_OscAesFile.hpp \
    $${PWD}/C_OscZipData.hpp \
    $${PWD}/C_OscZipFile.hpp \
    $${PWD}/aes/AES.h \
    $${PWD}/miniz/miniz.h
}

# optional: imports
contains(opensyde_core_skip_modules, opensyde_core_skip_imports) {
   message("opensyde_core_skip_imports detected ... skipping package")
} else {
   message("opensyde_core_skip_imports not detected ... dragging in package")

   SOURCES += \
      $${PWD}/imports/C_OscImportEdsDcf.cpp \
      $${PWD}/imports/C_OscImportRamView.cpp \
      $${PWD}//kefex_diaglib/dl_kefex/CKFXDATFile.cpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXDEFProject.cpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXProjectOptions.cpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXVariableBase.cpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXVariableListBase.cpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXVariableLists.cpp

   HEADERS += \
      $${PWD}/imports/C_OscImportEdsDcf.hpp \
      $${PWD}/imports/C_OscImportRamView.hpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXVariableLists.hpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXDATFile.hpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXDEFProject.hpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXProjectOptions.hpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXVariableBase.hpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXVariableListBase.hpp \
      $${PWD}/kefex_diaglib/dl_kefex/CKFXVariableLists.hpp
}

# optional: file generation
contains(opensyde_core_skip_modules, opensyde_core_skip_code_generation) {
   message("opensyde_core_skip_code_generation detected ... skipping package")
} else {
   message("opensyde_core_skip_code_generation not detected ... dragging in package")

   INCLUDEPATH += \
               $${PWD}/exports/code_generation

   SOURCES += \
    $${PWD}/exports/code_generation/C_OscExportCanOpenConfig.cpp \
    $${PWD}/exports/code_generation/C_OscExportCanOpenInit.cpp \
    $${PWD}/exports/code_generation/C_OscExportCommunicationStack.cpp \
    $${PWD}/exports/code_generation/C_OscExportDataPool.cpp \
    $${PWD}/exports/code_generation/C_OscExportHalc.cpp \
    $${PWD}/exports/code_generation/C_OscExportNode.cpp \
    $${PWD}/exports/code_generation/C_OscExportOsyInit.cpp \
    $${PWD}/exports/code_generation/C_OscExportUti.cpp \
    $${PWD}/exports/code_generation/C_OscExportParamSet.cpp

   HEADERS += \
    $${PWD}/exports/code_generation/C_OscExportCanOpenConfig.hpp \
    $${PWD}/exports/code_generation/C_OscExportCanOpenInit.hpp \
    $${PWD}/exports/code_generation/C_OscExportCommunicationStack.hpp \
    $${PWD}/exports/code_generation/C_OscExportDataPool.hpp \
    $${PWD}/exports/code_generation/C_OscExportHalc.hpp \
    $${PWD}/exports/code_generation/C_OscExportNode.hpp \
    $${PWD}/exports/code_generation/C_OscExportOsyInit.hpp \
    $${PWD}/exports/code_generation/C_OscExportUti.hpp \
    $${PWD}/exports/code_generation/C_OscExportParamSet.hpp
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
                  $${PWD}/imports \
                  $${PWD}/project/system \
                  $${PWD}/project/system/device_definition \
                  $${PWD}/project/system/FileLoadersV2 \
                  $${PWD}/project/system/node \
                  $${PWD}/project/system/node/can \
                  $${PWD}/project/system/node/can/can_open \
                  $${PWD}/project/system/node/data_logger \
                  $${PWD}/project/system/target_support_package \
                  $${PWD}/project/view

   SOURCES += \
       $${PWD}/halc/configuration/C_OscHalcConfig.cpp \
       $${PWD}/halc/configuration/C_OscHalcConfigChannel.cpp \
       $${PWD}/halc/configuration/C_OscHalcConfigDomain.cpp \
       $${PWD}/halc/configuration/C_OscHalcConfigFiler.cpp \
       $${PWD}/halc/configuration/C_OscHalcConfigParameter.cpp \
       $${PWD}/halc/configuration/C_OscHalcConfigParameterStruct.cpp \
       $${PWD}/halc/configuration/C_OscHalcConfigUtil.cpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandalone.cpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandaloneChannel.cpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandaloneDomain.cpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandaloneFiler.cpp \
       $${PWD}/halc/definition/base/C_OscHalcDefBase.cpp \
       $${PWD}/halc/definition/C_OscHalcDef.cpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelAvailability.cpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelDef.cpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelUseCase.cpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelValues.cpp \
       $${PWD}/halc/definition/C_OscHalcDefContent.cpp \
       $${PWD}/halc/definition/C_OscHalcDefContentBitmaskItem.cpp \
       $${PWD}/halc/definition/C_OscHalcDefDomain.cpp \
       $${PWD}/halc/definition/C_OscHalcDefElement.cpp \
       $${PWD}/halc/definition/C_OscHalcDefFiler.cpp \
       $${PWD}/halc/definition/C_OscHalcDefStruct.cpp \
       $${PWD}/halc/definition/C_OscHalcDefStructFiler.cpp \
       $${PWD}/halc/magician/C_OscHalcMagicianDatapoolListHandler.cpp \
       $${PWD}/halc/magician/C_OscHalcMagicianGenerator.cpp \
       $${PWD}/halc/magician/C_OscHalcMagicianUtil.cpp \
       $${PWD}/project/C_OscProject.cpp \
       $${PWD}/project/C_OscProjectFiler.cpp \
       $${PWD}/project/system/C_OscDeviceGroup.cpp \
       $${PWD}/project/system/C_OscDeviceManager.cpp \
       $${PWD}/project/system/C_OscSystemBus.cpp \
       $${PWD}/project/system/C_OscSystemBusFiler.cpp \
       $${PWD}/project/system/C_OscSystemDefinition.cpp \
       $${PWD}/project/system/C_OscSystemDefinitionFiler.cpp \
       $${PWD}/project/system/C_OscSystemNameMaxCharLimitChangeReportItem.cpp \
       $${PWD}/project/system/C_OscSystemFilerUtil.cpp \
       $${PWD}/project/system/device_definition/C_OscDeviceDefinitionFiler.cpp \
       $${PWD}/project/system/device_definition/C_OscDeviceDefinitionFilerV1.cpp \
       $${PWD}/project/system/device_definition/C_OscDeviceDefinition.cpp \
       $${PWD}/project/system/device_definition/C_OscSupportedCanInterfaceFeatures.cpp \
       $${PWD}/project/system/device_definition/C_OscSubDeviceDefinition.cpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackage.cpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackageFiler.cpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackageV2.cpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackageV2Filer.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OscNodeDataPoolFilerV2.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OscNodeFilerV2.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OscSystemBusFilerV2.cpp \
       $${PWD}/project/system/FileLoadersV2/C_OscSystemDefinitionFilerV2.cpp \
       $${PWD}/project/system/node/can/C_OscCanMessage.cpp \
       $${PWD}/project/system/node/can/C_OscCanMessageUniqueId.cpp \
       $${PWD}/project/system/node/can/C_OscCanMessageContainer.cpp \
       $${PWD}/project/system/node/can/C_OscCanMessageIdentificationIndices.cpp \
       $${PWD}/project/system/node/can/C_OscCanProtocol.cpp \
       $${PWD}/project/system/node/can/C_OscCanSignal.cpp \
       $${PWD}/project/system/node/can/C_OscCanUtil.cpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerJob.cpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerJobFiler.cpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerJobProperties.cpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerDataElementReference.cpp \
       $${PWD}/project/system/node/C_OscNode.cpp \
       $${PWD}/project/system/node/C_OscNodeSquad.cpp \
       $${PWD}/project/system/node/C_OscNodeSquadFiler.cpp \
       $${PWD}/project/system/node/C_OscNodeApplication.cpp \
       $${PWD}/project/system/node/C_OscNodeCodeExportSettings.cpp \
       $${PWD}/project/system/node/C_OscNodeComInterfaceSettings.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPool.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolContent.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolContentUtil.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolDataSet.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolFiler.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolList.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListElement.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListElementId.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListElementOptArrayId.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListId.cpp \
       $${PWD}/project/system/node/C_OscNodeFiler.cpp \
       $${PWD}/project/system/node/C_OscNodeOpenSydeServerSettings.cpp \
       $${PWD}/project/system/node/C_OscNodeProperties.cpp \
       $${PWD}/project/system/node/C_OscNodeStwFlashloaderSettings.cpp \
       $${PWD}/project/system/node/C_OscNodeCommFiler.cpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolId.cpp \
       $${PWD}/project/system/node/can/C_OscCanInterfaceId.cpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerMappableSignal.cpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerDeviceInfo.cpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerInfo.cpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerFiler.cpp \
       $${PWD}/project/view/C_OscViewPc.cpp \
       $${PWD}/project/view/C_OscViewData.cpp \
       $${PWD}/project/view/C_OscViewNodeUpdate.cpp \
       $${PWD}/project/view/C_OscViewNodeUpdateParamInfo.cpp \
       $${PWD}/project/view/C_OscViewFiler.cpp \
       $${PWD}/imports/C_OscEdsDcfImportMessageGroup.cpp \
       $${PWD}/imports/C_OscCanOpenObjectDictionary.cpp \
       $${PWD}/imports/C_OscCanOpenEdsInfoBlock.cpp \
       $${PWD}/imports/C_OscCanOpenEdsDeviceInfoBlock.cpp \
       $${PWD}/imports/C_OscCanOpenEdsFileInfoBlock.cpp

   HEADERS += \
       $${PWD}/halc/configuration/C_OscHalcConfig.hpp \
       $${PWD}/halc/configuration/C_OscHalcConfigChannel.hpp \
       $${PWD}/halc/configuration/C_OscHalcConfigDomain.hpp \
       $${PWD}/halc/configuration/C_OscHalcConfigFiler.hpp \
       $${PWD}/halc/configuration/C_OscHalcConfigParameter.hpp \
       $${PWD}/halc/configuration/C_OscHalcConfigParameterStruct.hpp \
       $${PWD}/halc/configuration/C_OscHalcConfigUtil.hpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandalone.hpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandaloneChannel.hpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandaloneDomain.hpp \
       $${PWD}/halc/configuration/standalone/C_OscHalcConfigStandaloneFiler.hpp \
       $${PWD}/halc/definition/base/C_OscHalcDefBase.hpp \
       $${PWD}/halc/definition/C_OscHalcDef.hpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelAvailability.hpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelDef.hpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelUseCase.hpp \
       $${PWD}/halc/definition/C_OscHalcDefChannelValues.hpp \
       $${PWD}/halc/definition/C_OscHalcDefContent.hpp \
       $${PWD}/halc/definition/C_OscHalcDefContentBitmaskItem.hpp \
       $${PWD}/halc/definition/C_OscHalcDefDomain.hpp \
       $${PWD}/halc/definition/C_OscHalcDefElement.hpp \
       $${PWD}/halc/definition/C_OscHalcDefFiler.hpp \
       $${PWD}/halc/definition/C_OscHalcDefStruct.hpp \
       $${PWD}/halc/definition/C_OscHalcDefStructFiler.hpp \
       $${PWD}/halc/magician/C_OscHalcMagicianDatapoolListHandler.hpp \
       $${PWD}/halc/magician/C_OscHalcMagicianGenerator.hpp \
       $${PWD}/halc/magician/C_OscHalcMagicianUtil.hpp \
       $${PWD}/project/C_OscProject.hpp \
       $${PWD}/project/C_OscProjectFiler.hpp \
       $${PWD}/project/system/C_OscDeviceGroup.hpp \
       $${PWD}/project/system/C_OscDeviceManager.hpp \
       $${PWD}/project/system/C_OscSystemBus.hpp \
       $${PWD}/project/system/C_OscSystemBusFiler.hpp \
       $${PWD}/project/system/C_OscSystemDefinition.hpp \
       $${PWD}/project/system/C_OscSystemDefinitionFiler.hpp \
       $${PWD}/project/system/C_OscSystemNameMaxCharLimitChangeReportItem.hpp \
       $${PWD}/project/system/C_OscSystemFilerUtil.hpp \
       $${PWD}/project/system/device_definition/C_OscDeviceDefinition.hpp \
       $${PWD}/project/system/device_definition/C_OscDeviceDefinitionFiler.hpp \
       $${PWD}/project/system/device_definition/C_OscDeviceDefinitionFilerV1.hpp \
       $${PWD}/project/system/device_definition/C_OscSupportedCanInterfaceFeatures.hpp \
       $${PWD}/project/system/device_definition/C_OscSubDeviceDefinition.hpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackage.hpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackageFiler.hpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackageV2.hpp \
       $${PWD}/project/system/target_support_package/C_OscTargetSupportPackageV2Filer.hpp \
       $${PWD}/project/system/FileLoadersV2/C_OscNodeDataPoolFilerV2.hpp \
       $${PWD}/project/system/FileLoadersV2/C_OscNodeFilerV2.hpp \
       $${PWD}/project/system/FileLoadersV2/C_OscSystemBusFilerV2.hpp \
       $${PWD}/project/system/FileLoadersV2/C_OscSystemDefinitionFilerV2.hpp \
       $${PWD}/project/system/node/C_OscNode.hpp \
       $${PWD}/project/system/node/C_OscNodeSquad.hpp \
       $${PWD}/project/system/node/C_OscNodeSquadFiler.hpp \
       $${PWD}/project/system/node/C_OscNodeApplication.hpp \
       $${PWD}/project/system/node/C_OscNodeCodeExportSettings.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolContentUtil.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolId.hpp \
       $${PWD}/project/system/node/C_OscNodeComInterfaceSettings.hpp \
       $${PWD}/project/system/node/C_OscNodeCommFiler.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPool.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolContent.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolDataSet.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolFiler.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolList.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListElement.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListElementId.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListElementOptArrayId.hpp \
       $${PWD}/project/system/node/C_OscNodeDataPoolListId.hpp \
       $${PWD}/project/system/node/C_OscNodeFiler.hpp \
       $${PWD}/project/system/node/C_OscNodeOpenSydeServerSettings.hpp \
       $${PWD}/project/system/node/C_OscNodeProperties.hpp \
       $${PWD}/project/system/node/C_OscNodeStwFlashloaderSettings.hpp \
       $${PWD}/project/system/node/can/C_OscCanMessage.hpp \
       $${PWD}/project/system/node/can/C_OscCanMessageUniqueId.hpp \
       $${PWD}/project/system/node/can/C_OscCanMessageContainer.hpp \
       $${PWD}/project/system/node/can/C_OscCanMessageIdentificationIndices.hpp \
       $${PWD}/project/system/node/can/C_OscCanProtocol.hpp \
       $${PWD}/project/system/node/can/C_OscCanSignal.hpp \
       $${PWD}/project/system/node/can/C_OscCanUtil.hpp \
       $${PWD}/project/system/node/can/C_OscCanInterfaceId.hpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerMappableSignal.hpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerDeviceInfo.hpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerInfo.hpp \
       $${PWD}/project/system/node/can/can_open/C_OscCanOpenManagerFiler.hpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerJob.hpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerJobFiler.hpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerJobProperties.hpp \
       $${PWD}/project/system/node/data_logger/C_OscDataLoggerDataElementReference.hpp \
       $${PWD}/project/view/C_OscViewData.hpp \
       $${PWD}/project/view/C_OscViewPc.hpp \
       $${PWD}/project/view/C_OscViewNodeUpdate.hpp \
       $${PWD}/project/view/C_OscViewNodeUpdateParamInfo.hpp \
       $${PWD}/project/view/C_OscViewFiler.hpp \
       $${PWD}/imports/C_OscEdsDcfImportMessageGroup.hpp \
       $${PWD}/imports/C_OscCanOpenObjectDictionary.hpp \
       $${PWD}/imports/C_OscCanOpenEdsInfoBlock.hpp \
       $${PWD}/imports/C_OscCanOpenEdsDeviceInfoBlock.hpp \
       $${PWD}/imports/C_OscCanOpenEdsFileInfoBlock.hpp
}

# optional: parameter set handling (reading and writing syde_psi files)
contains(opensyde_core_skip_modules, opensyde_core_skip_param_set) {
   message("opensyde_core_skip_param_set detected ... skipping package")
} else {
   message("opensyde_core_skip_param_set not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/data_dealer/paramset

   SOURCES += $${PWD}/data_dealer/paramset/C_OscParamSetDataPoolInfo.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetFilerBase.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetHandler.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedData.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedDataPool.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedElement.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedFileInfoData.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedList.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedNode.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedNodeFiler.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetRawEntry.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetRawNode.cpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetRawNodeFiler.cpp

   HEADERS += $${PWD}/data_dealer/paramset/C_OscParamSetDataPoolInfo.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetFilerBase.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetHandler.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedData.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedDataPool.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedElement.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedFileInfoData.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedList.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedNode.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetInterpretedNodeFiler.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetRawEntry.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetRawNode.hpp \
              $${PWD}/data_dealer/paramset/C_OscParamSetRawNodeFiler.hpp
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

   SOURCES += $${PWD}/can_dispatcher/dispatcher/C_CanBase.cpp \
            $${PWD}/can_dispatcher/dispatcher/C_CanDispatcher.cpp \
            $${PWD}/can_dispatcher/target_windows_stw_dlls/C_Can.cpp \
            $${PWD}/can_dispatcher/target_windows_stw_dlls/C_CanDll.cpp \
            $${PWD}/ip_dispatcher/target_windows_win_sock/C_OscIpDispatcherWinSock.cpp

   HEADERS += $${PWD}/can_dispatcher/dispatcher/stw_can.hpp \
              $${PWD}/can_dispatcher/dispatcher/C_CanBase.hpp \
              $${PWD}/can_dispatcher/dispatcher/C_CanDispatcher.hpp \
              $${PWD}/can_dispatcher/target_windows_stw_dlls/C_Can.hpp \
              $${PWD}/can_dispatcher/target_windows_stw_dlls/C_CanDll.hpp \
              $${PWD}/ip_dispatcher/dispatcher/C_OscIpDispatcher.hpp \
              $${PWD}/ip_dispatcher/target_windows_win_sock/C_OscIpDispatcherWinSock.hpp
}


# optional: protocol drivers common (openSYDE, STW Flashloader, KEFEX, Data Dealer)
# Also requires CAN and Ethernet dispatchers
contains(opensyde_core_skip_modules, opensyde_core_skip_protocol_drivers_common) {
   message("opensyde_core_skip_protocol_drivers_common detected ... skipping package")
} else {
   message("opensyde_core_skip_protocol_drivers_common not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/protocol_drivers \
                  $${PWD}/protocol_drivers/communication \
                  $${PWD}/protocol_drivers/device_config \
                  $${PWD}/kefex_diaglib/dl_pool \
                  $${PWD}/kefex_diaglib/hexfile \
                  $${PWD}/kefex_diaglib/dl_kefex \
                  $${PWD}/kefex_diaglib/dl_stwflash

   SOURCES += $${PWD}/kefex_diaglib/dl_pool/CDLReportEvents.cpp \
              $${PWD}/kefex_diaglib/hexfile/C_HexFile.cpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXComm.cpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXCommConfiguration.cpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXCommunicationKEFEX.cpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXProtocol.cpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLActions.cpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLECUInformation.cpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLFlashWrite.cpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLFlashWriteParameters.cpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLHexFile.cpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLProtocol.cpp \
              $${PWD}/protocol_drivers/C_OscCanDispatcherOsyRouter.cpp \
              $${PWD}/protocol_drivers/C_OscDiagProtocolBase.cpp \
              $${PWD}/protocol_drivers/C_OscDiagProtocolKfx.cpp \
              $${PWD}/protocol_drivers/C_OscDiagProtocolOsy.cpp \
              $${PWD}/protocol_drivers/C_OscFlashProtocolStwFlashloader.cpp \
              $${PWD}/protocol_drivers/C_OscProtocolSerialNumber.cpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsy.cpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsyTpBase.cpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsyTpCan.cpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsyTpIp.cpp \
              $${PWD}/protocol_drivers/C_OscUpdateUtil.cpp \
              $${PWD}/protocol_drivers/C_OscHexFile.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComFlashloaderInformation.cpp \
              $${PWD}/protocol_drivers/device_config/C_OscDcDeviceInformation.cpp

   HEADERS += $${PWD}/kefex_diaglib/dl_pool/CDLReportEvents.hpp \
              $${PWD}/kefex_diaglib/hexfile/C_HexFile.hpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXComm.hpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXCommConfiguration.hpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXCommunicationKEFEX.hpp \
              $${PWD}/kefex_diaglib/dl_kefex/CKFXProtocol.hpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLActions.hpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLECUInformation.hpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLFlashWrite.hpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLFlashWriteParameters.hpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLHexFile.hpp \
              $${PWD}/kefex_diaglib/dl_stwflash/CXFLProtocol.hpp \
              $${PWD}/protocol_drivers/C_OscCanDispatcherOsyRouter.hpp \
              $${PWD}/protocol_drivers/C_OscDiagProtocolBase.hpp \
              $${PWD}/protocol_drivers/C_OscDiagProtocolKfx.hpp \
              $${PWD}/protocol_drivers/C_OscDiagProtocolOsy.hpp \
              $${PWD}/protocol_drivers/C_OscFlashProtocolStwFlashloader.hpp \
              $${PWD}/protocol_drivers/C_OscProtocolSerialNumber.hpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsy.hpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsyTpBase.hpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsyTpCan.hpp \
              $${PWD}/protocol_drivers/C_OscProtocolDriverOsyTpIp.hpp \
              $${PWD}/protocol_drivers/C_OscUpdateUtil.hpp \
              $${PWD}/protocol_drivers/C_OscHexFile.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComFlashloaderInformation.hpp \
              $${PWD}/protocol_drivers/device_config/C_OscDcDeviceInformation.hpp
}

# optional: protocol drivers basic(openSYDE, STW Flashloader, KEFEX, Data Dealer)
# Also requires CAN and Ethernet dispatchers
contains(opensyde_core_skip_modules, opensyde_core_skip_protocol_drivers_basic) {
   message("opensyde_core_skip_protocol_drivers_basic detected ... skipping package")
} else {
   message("opensyde_core_skip_protocol_drivers_basic not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/protocol_drivers/basic_update \
                  $${PWD}/protocol_drivers/device_config

   SOURCES += $${PWD}/protocol_drivers/basic_update/C_OscBuSequences.cpp \
              $${PWD}/protocol_drivers/device_config/C_OscDcBasicSequences.cpp

   HEADERS += $${PWD}/protocol_drivers/basic_update/C_OscBuSequences.hpp \
              $${PWD}/protocol_drivers/device_config/C_OscDcBasicSequences.hpp
}

# optional: x-certificates generation functionality
contains(opensyde_core_skip_modules, opensyde_core_skip_x_certificates_generation) {
   message("opensyde_core_skip_x_certificates_generation detected ... skipping package")
} else {
   message("opensyde_core_skip_x_certificates_generation not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/exports/x_certificates_package_generation

   SOURCES += $${PWD}/exports/x_certificates_package_generation/C_OscXceManifest.cpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceManifestFiler.cpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceUpdatePackageParameters.cpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceBase.cpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceCreate.cpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceLoad.cpp \

   HEADERS += $${PWD}/exports/x_certificates_package_generation/C_OscXceManifest.hpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceManifestFiler.hpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceUpdatePackageParameters.hpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceBase.hpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceCreate.hpp \
              $${PWD}/exports/x_certificates_package_generation/C_OscXceLoad.hpp \
}

# optional: system package base functionality
contains(opensyde_core_skip_modules, opensyde_core_skip_system_package_handling) {
   message("opensyde_core_skip_system_package_handling detected ... skipping package")
} else {
   message("opensyde_core_skip_system_package_handling not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/system_package_handling

   SOURCES += $${PWD}/system_package_handling/C_OscSpaServicePackageCreateUtil.cpp \
              $${PWD}/system_package_handling/C_OscSpaServicePackageLoadUtil.cpp \

   HEADERS += $${PWD}/system_package_handling/C_OscSpaServicePackageCreateUtil.hpp \
              $${PWD}/system_package_handling/C_OscSpaServicePackageLoadUtil.hpp \
}

# optional: x-config generation functionality
contains(opensyde_core_skip_modules, opensyde_core_skip_x_config_generation) {
   message("opensyde_core_skip_x_config_generation detected ... skipping package")
} else {
   message("opensyde_core_skip_x_config_generation not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/exports/x_config_generation

   SOURCES += $${PWD}/exports/x_config_generation/C_OscXcoBase.cpp \
              $${PWD}/exports/x_config_generation/C_OscXcoCreate.cpp \
              $${PWD}/exports/x_config_generation/C_OscXcoLoad.cpp \
              $${PWD}/exports/x_config_generation/C_OscXcoManifest.cpp \
              $${PWD}/exports/x_config_generation/C_OscXcoManifestFiler.cpp \

   HEADERS += $${PWD}/exports/x_config_generation/C_OscXcoBase.hpp \
              $${PWD}/exports/x_config_generation/C_OscXcoCreate.hpp \
              $${PWD}/exports/x_config_generation/C_OscXcoLoad.hpp \
              $${PWD}/exports/x_config_generation/C_OscXcoManifest.hpp \
              $${PWD}/exports/x_config_generation/C_OscXcoManifestFiler.hpp \
}

# optional: protocol drivers system(openSYDE, STW Flashloader, KEFEX, Data Dealer)
# Also requires CAN and Ethernet dispatchers
contains(opensyde_core_skip_modules, opensyde_core_skip_protocol_drivers_system) {
   message("opensyde_core_skip_protocol_drivers_system detected ... skipping package")
} else {
   message("opensyde_core_skip_protocol_drivers_system not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/data_dealer \
                  $${PWD}/security \
                  $${PWD}/system_update_package \
                  $${PWD}/protocol_drivers \
                  $${PWD}/protocol_drivers/basic_update \
                  $${PWD}/protocol_drivers/routing \
                  $${PWD}/protocol_drivers/communication \
                  $${PWD}/protocol_drivers/system_update \
                  $${PWD}/kefex_diaglib/dl_pool \
                  $${PWD}/kefex_diaglib/hexfile \
                  $${PWD}/kefex_diaglib/dl_kefex \
                  $${PWD}/kefex_diaglib/dl_stwflash \
                  $${PWD}/kefex_diaglib/cmonprotocol

   SOURCES += $${PWD}/data_dealer/C_OscDataDealer.cpp \
              $${PWD}/data_dealer/C_OscDataDealerNvm.cpp \
              $${PWD}/data_dealer/C_OscDataDealerNvmSafe.cpp \
              $${PWD}/security/C_OscSecurityPem.cpp \
              $${PWD}/security/C_OscSecurityPemBase.cpp \
              $${PWD}/security/C_OscSecurityPemDatabase.cpp \
              $${PWD}/security/C_OscSecurityPemKeyInfo.cpp \
              $${PWD}/security/C_OscSecurityPemSecUpdate.cpp \
              $${PWD}/security/C_OscSecurityEcdsa.cpp \
              $${PWD}/security/C_OscSecurityRsa.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComDriverBase.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComDriverFlash.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComDriverProtocol.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComSequencesBase.cpp \
              $${PWD}/protocol_drivers/routing/C_OscRoutingCalculation.cpp \
              $${PWD}/protocol_drivers/routing/C_OscRoutingRoute.cpp \
              $${PWD}/protocol_drivers/system_update/C_OscSuSequences.cpp \
              $${PWD}/protocol_drivers/system_update/C_OscSuSequencesNodeStates.cpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageV1.cpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageCreate.cpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageLoad.cpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageBase.cpp \
              $${PWD}/system_update_package/C_OscSupSignatureFiler.cpp \
              $${PWD}/system_update_package/C_OscSupDefinition.cpp \
              $${PWD}/system_update_package/C_OscSupDefinitionFiler.cpp \
              $${PWD}/system_update_package/C_OscSupNodeDefinition.cpp \
              $${PWD}/system_update_package/C_OscSupNodeDefinitionFiler.cpp \

   HEADERS += $${PWD}/data_dealer/C_OscDataDealer.hpp \
              $${PWD}/data_dealer/C_OscDataDealerNvm.hpp \
              $${PWD}/data_dealer/C_OscDataDealerNvmSafe.hpp \
              $${PWD}/security/C_OscSecurityPem.hpp \
              $${PWD}/security/C_OscSecurityPemBase.hpp \
              $${PWD}/security/C_OscSecurityPemDatabase.hpp \
              $${PWD}/security/C_OscSecurityPemKeyInfo.hpp \
              $${PWD}/security/C_OscSecurityPemSecUpdate.hpp \
              $${PWD}/security/C_OscSecurityEcdsa.hpp \
              $${PWD}/security/C_OscSecurityRsa.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComDriverBase.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComDriverFlash.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComDriverProtocol.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComSequencesBase.hpp \
              $${PWD}/protocol_drivers/routing/C_OscRoutingCalculation.hpp \
              $${PWD}/protocol_drivers/routing/C_OscRoutingRoute.hpp \
              $${PWD}/protocol_drivers/system_update/C_OscSuSequences.hpp \
              $${PWD}/protocol_drivers/system_update/C_OscSuSequencesNodeStates.hpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageV1.hpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageCreate.hpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageLoad.hpp \
              $${PWD}/system_update_package/C_OscSupServiceUpdatePackageBase.hpp \
              $${PWD}/system_update_package/C_OscSupSignatureFiler.hpp \
              $${PWD}/system_update_package/C_OscSupDefinition.hpp \
              $${PWD}/system_update_package/C_OscSupDefinitionFiler.hpp \
              $${PWD}/system_update_package/C_OscSupNodeDefinition.hpp \
              $${PWD}/system_update_package/C_OscSupNodeDefinitionFiler.hpp \
}

# optional: protocol logging
# Also requires CAN and Ethernet dispatchers
# Note that C_OscComDriverBase.cpp is part of this package and of the protocol drivers as it's needed in both cases
contains(opensyde_core_skip_modules, opensyde_core_skip_protocol_logging) {
   message("opensyde_core_skip_protocol_logging detected ... skipping package")
} else {
   message("opensyde_core_skip_protocol_logging not detected ... dragging in package")

   INCLUDEPATH += $${PWD}/kefex_diaglib \
                  $${PWD}/kefex_diaglib/cmonprotocol \
                  $${PWD}/protocol_drivers/communication

   SOURCES += $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocol.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolBase.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolCanOpen.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolGd.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolJ1939.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolKefex.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolL2.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolOpenSyde.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolShipIpIva.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolStwFf.cpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolXfl.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComLoggerProtocolOpenSyde.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComLoggerProtocols.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLogger.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerData.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerFileAsc.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerFileBase.cpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerOsySysDefConfig.cpp

   HEADERS += $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocol.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolBase.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolCanOpen.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolGd.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolJ1939.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolKefex.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolL2.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolOpenSyde.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolShipIpIva.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolStwFf.hpp \
              $${PWD}/kefex_diaglib/cmonprotocol/C_CanMonProtocolXfl.hpp \
              $${PWD}/kefex_diaglib/C_CanMonProtocolTarget.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComLoggerProtocolOpenSyde.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComLoggerProtocols.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLogger.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerData.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerFileAsc.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerFileBase.hpp \
              $${PWD}/protocol_drivers/communication/C_OscComMessageLoggerOsySysDefConfig.hpp

   ! contains(SOURCES, $${PWD}/protocol_drivers/communication/C_OscComDriverBase.cpp) {
      SOURCES += $${PWD}/protocol_drivers/communication/C_OscComDriverBase.cpp
      HEADERS += $${PWD}/protocol_drivers/communication/C_OscComDriverBase.hpp
   }

}
