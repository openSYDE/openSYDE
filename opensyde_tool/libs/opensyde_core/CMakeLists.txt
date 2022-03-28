# This file lists all openSYDE Core files and include paths.
# It can be included from the CMakeLists.txt file of an application.
# Some optional parts of the core can be skipped by setting options in the application CMakeLists.txt file before
#  including this file in order to keep build times low.
# See the "OPENSYDE_CORE_SKIP_*" checks below for parts that can be skipped
#
# This CMakeLists.txt defined the library "opensyde_core" to be linked from the application.
# As the openSYDE core needs the "precomp_headers.h" that is placed with the application the path
#  to that file needs to be set before this CMakeLists.txt is dragged in.
#
# Simplified example usage:
#
# <example>
#
# include_directories(
#    ../src
# )
#
# set(OPENSYDE_CORE_SKIP_ZIPPING 1)  #we do not want the zipping stuff
#
# add_subdirectory(../libs/opensyde_core ${CMAKE_BINARY_DIR}/opensyde_core)
#
# add_executable(my_application mymain.cpp)
#
# target_link_libraries(my_application
#   opensyde_core
# )
# </example>

# minimum 3.16 for precompiled headers
cmake_minimum_required(VERSION 3.16)
project("opensyde_core")

# --- Project source files ---------------------------------------------------------------------------------------------
#basic infrastructure (also requires Windows target layer, or a corresponding implementation for e.g. Linux)
set(OPENSYDE_CORE_COMMON_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCBinaryHash.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCChecksummedIniFile.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCUtils.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CSCLResourceStrings.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/DLStrings.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/stwcompid.c
   ${CMAKE_CURRENT_SOURCE_DIR}/logging/C_OSCLoggingHandler.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/md5/CMD5Checksum.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLChecksums.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLDateTime.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLIniFile.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLString.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLStringList.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OSCXMLParser.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OSCXMLParserLog.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OSCChecksummedXML.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/tinyxml2/tinyxml2.cpp

   ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCBinaryHash.h
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCChecksummedIniFile.h
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCUtils.h
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CSCLResourceStrings.h
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/DLStrings.h
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/stwcompid.h
   ${CMAKE_CURRENT_SOURCE_DIR}/logging/C_OSCLoggingHandler.h
   ${CMAKE_CURRENT_SOURCE_DIR}/md5/CMD5Checksum.h
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLChecksums.h
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLDateTime.h
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLIniFile.h
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLString.h
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/CSCLStringList.h
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/SCLDynamicArray.h
   ${CMAKE_CURRENT_SOURCE_DIR}/stwerrors.h
   ${CMAKE_CURRENT_SOURCE_DIR}/stwtypes/stwtypes.h
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OSCXMLParser.h
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OSCXMLParserLog.h
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OSCChecksummedXML.h
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/tinyxml2/tinyxml2.h
)

set(OPENSYDE_CORE_COMMON_INCLUDE_DIRECTORIES
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib
   ${CMAKE_CURRENT_SOURCE_DIR}/logging
   ${CMAKE_CURRENT_SOURCE_DIR}/md5
   ${CMAKE_CURRENT_SOURCE_DIR}/scl
   ${CMAKE_CURRENT_SOURCE_DIR}/stwtypes
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/tinyxml2
)

# zip/unzip functionality
if(OPENSYDE_CORE_SKIP_ZIPPING)
   message("OPENSYDE_CORE_SKIP_ZIPPING detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_ZIPPING not detected ... dragging in package")

   set(OPENSYDE_CORE_ZIPPING_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCAesFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCZipData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCZipFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/aes/AES.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/miniz/miniz.c
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCAesFile.h
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCZipData.h
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OSCZipFile.h
      ${CMAKE_CURRENT_SOURCE_DIR}/aes/AES.h
      ${CMAKE_CURRENT_SOURCE_DIR}/miniz/miniz.h
   )

   set(OPENSYDE_CORE_ZIPPING_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/aes
      ${CMAKE_CURRENT_SOURCE_DIR}/miniz
   )
endif()

# imports
if(OPENSYDE_CORE_SKIP_IMPORTS)
   message("OPENSYDE_CORE_SKIP_IMPORTS detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_IMPORTS not detected ... dragging in package")

   set(OPENSYDE_CORE_IMPORTS_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OSCCanOpenObjectDictionary.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OSCImportEdsDcf.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OSCCanOpenObjectDictionary.h
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OSCImportEdsDcf.h
   )

   set(OPENSYDE_CORE_IMPORTS_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/imports
   )
endif()

# source code generation
if(OPENSYDE_CORE_SKIP_CODE_GENERATION)
   message("OPENSYDE_CORE_SKIP_CODE_GENERATION detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_CODE_GENERATION not detected ... dragging in package")

   set(OPENSYDE_CORE_CODE_GENERATION_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportCommunicationStack.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportDataPool.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportHalc.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportOsyInit.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportParamSet.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportUti.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportCommunicationStack.h
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportDataPool.h
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportHalc.h
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportNode.h
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportOsyInit.h
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportParamSet.h
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OSCExportUti.h
   )

   set(OPENSYDE_CORE_CODE_GENERATION_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/exports
   )
endif()


# project handling (Including all aspects of System Definition)
if(OPENSYDE_CORE_SKIP_PROJECT_HANDLING)
   message("OPENSYDE_CORE_SKIP_PROJECT_HANDLING detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_PROJECT_HANDLING not detected ... dragging in package")

   set(OPENSYDE_CORE_PROJECT_HANDLING_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfig.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigChannel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigDomain.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigParameter.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigParameterStruct.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandalone.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandaloneChannel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandaloneDomain.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandaloneFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/base/C_OSCHalcDefBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDef.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelAvailability.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelDef.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelUseCase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelValues.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefContent.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefContentBitmaskItem.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefDomain.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefElement.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefStruct.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefStructFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OSCHALCMagicianDatapoolListHandler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OSCHALCMagicianGenerator.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OSCHALCMagicianUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OSCProject.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OSCProjectFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceDefinitionFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceDefinitionFilerV1.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceDefinition.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSubDeviceDefinition.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceGroup.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceManager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemBus.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemBusFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemDefinition.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemDefinitionFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemFilerUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCTargetSupportPackage.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCTargetSupportPackageFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCNodeDataPoolFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCNodeFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCSystemBusFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCSystemDefinitionFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessage.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessageUniqueId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessageContainer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessageIdentificationIndices.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanSignal.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeSquad.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeSquadFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeApplication.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeCodeExportSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeComInterfaceSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPool.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolContent.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolContentUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolDataSet.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolList.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolListElement.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolListElementId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolListId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeOpenSydeServerSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeProperties.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeStwFlashloaderSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeCommFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolId.cpp

      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfig.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigChannel.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigDomain.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigParameter.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigParameterStruct.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OSCHalcConfigUtil.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandalone.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandaloneChannel.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandaloneDomain.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OSCHalcConfigStandaloneFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/base/C_OSCHalcDefBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDef.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelAvailability.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelDef.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelUseCase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefChannelValues.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefContent.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefContentBitmaskItem.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefDomain.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefElement.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefStruct.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OSCHalcDefStructFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OSCHALCMagicianDatapoolListHandler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OSCHALCMagicianGenerator.h
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OSCHALCMagicianUtil.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OSCProject.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OSCProjectFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceDefinition.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSubDeviceDefinition.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceDefinitionFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceDefinitionFilerV1.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceGroup.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCDeviceManager.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemBus.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemBusFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemDefinition.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemDefinitionFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCSystemFilerUtil.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCTargetSupportPackage.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OSCTargetSupportPackageFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCNodeDataPoolFilerV2.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCNodeFilerV2.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCSystemBusFilerV2.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OSCSystemDefinitionFilerV2.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNode.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeSquad.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeSquadFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeApplication.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeCodeExportSettings.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolContentUtil.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolId.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeComInterfaceSettings.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeCommFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPool.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolContent.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolDataSet.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolList.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolListElement.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolListElementId.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeDataPoolListId.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeOpenSydeServerSettings.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeProperties.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OSCNodeStwFlashloaderSettings.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessage.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessageUniqueId.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessageContainer.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanMessageIdentificationIndices.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanProtocol.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanSignal.h
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OSCCanUtil.h
   )

   set(OPENSYDE_CORE_PROJECT_HANDLING_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/halc
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/base
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician
      ${CMAKE_CURRENT_SOURCE_DIR}/project
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can
   )
endif()

# param set handling (reading and writing syde_psi files)
if(OPENSYDE_CORE_SKIP_PARAM_SET)
   message("OPENSYDE_CORE_SKIP_PARAM_SET detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_PARAM_SET not detected ... dragging in package")

   set(OPENSYDE_CORE_PARAM_SET_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetDataPoolInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetFilerBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetHandler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedDataPool.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedElement.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedFileInfoData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedList.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedNodeFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetRawEntry.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetRawNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetRawNodeFiler.cpp

      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetDataPoolInfo.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetFilerBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetHandler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedData.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedDataPool.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedElement.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedFileInfoData.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedList.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedNode.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetInterpretedNodeFiler.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetRawEntry.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetRawNode.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OSCParamSetRawNodeFiler.h
   )

   set(OPENSYDE_CORE_PARAM_SET_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset
   )
endif()

# communication protocol drivers (openSYDE, STW Flashloader, KEFEX, Data Dealer, Dispatcher interface)
if(OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS)
   message("OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_PROTOCOL_DRIVERS not detected ... dragging in package")

   set(OPENSYDE_CORE_PROTOCOL_DRIVERS_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/CCANBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/CCANDispatcher.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OSCDataDealer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OSCDataDealerNvm.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OSCDataDealerNvmSafe.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CDLReportEvents.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CHexFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXComm.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXCommConfiguration.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXCommunicationKEFEX.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLActions.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLECUInformation.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLFlashWrite.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLFlashWriteParameters.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLHexFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityPem.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityPemKeyInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityPemDatabase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityRsa.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCCanDispatcherOsyRouter.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCDiagProtocolBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCDiagProtocolKfx.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCDiagProtocolOsy.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCFlashProtocolStwFlashloader.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolSerialNumber.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsy.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsyTpBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsyTpCan.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsyTpIp.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComDriverBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComDriverFlash.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComDriverProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComSequencesBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OSCRoutingCalculation.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OSCRoutingRoute.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OSCSuSequences.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OSCSuSequencesNodeStates.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OsyHexFile.cpp

      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/CCANBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/CCANDispatcher.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OSCDataDealer.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OSCDataDealerNvm.h
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OSCDataDealerNvmSafe.h
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/dispatcher/C_OSCIpDispatcher.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CDLReportEvents.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CHexFile.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXComm.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXCommConfiguration.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXCommunicationKEFEX.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CKFXProtocol.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLActions.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLECUInformation.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLFlashWrite.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLFlashWriteParameters.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLHexFile.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CXFLProtocol.h
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityPem.h
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityPemKeyInfo.h
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityPemDatabase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OSCSecurityRsa.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCCanDispatcherOsyRouter.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCDiagProtocolBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCDiagProtocolKfx.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCDiagProtocolOsy.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCFlashProtocolStwFlashloader.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolSerialNumber.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsy.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsyTpBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsyTpCan.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OSCProtocolDriverOsyTpIp.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComDriverBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComDriverFlash.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComDriverProtocol.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComSequencesBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OSCRoutingCalculation.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OSCRoutingRoute.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OSCSuSequences.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OSCSuSequencesNodeStates.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OSCSuServiceUpdatePackage.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OsyHexFile.h
   )

   set(OPENSYDE_CORE_PROTOCOL_DRIVERS_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/dispatcher
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer
      ${CMAKE_CURRENT_SOURCE_DIR}/security
      ${CMAKE_CURRENT_SOURCE_DIR}/libtomcrypt/src/headers
      ${CMAKE_CURRENT_SOURCE_DIR}/libtomcrypt/libtommath
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib
   )

   add_subdirectory(libtomcrypt)
endif()

# windows target drivers: CAN and Ethernet dispatchers
if(OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS)
   message("OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS not detected ... dragging in package")

   set (OPENSYDE_CORE_WINDOWS_DRIVERS_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/CCAN.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/CCANDLL.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/CCAN.h
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/CCANDLL.h
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/target_windows_win_sock/C_OSCIpDispatcherWinSock.h
   )

   set(OPENSYDE_CORE_WINDOWS_DRIVERS_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/target_windows_win_sock
   )
endif()

# windows target glue layer
if(OPENSYDE_CORE_SKIP_WINDOWS_TARGET)
   message("OPENSYDE_CORE_SKIP_WINDOWS_TARGET detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_WINDOWS_TARGET not detected ... dragging in package")

   set (OPENSYDE_CORE_WINDOWS_TARGET_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLTasks.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLTime.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLUtils.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLFile.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLTasks.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLTime.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLUtils.h
   )

   set(OPENSYDE_CORE_WINDOWS_TARGET_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows
   )
endif()


# protocol logging
if(OPENSYDE_CORE_SKIP_PROTOCOL_LOGGING)
   message("OPENSYDE_CORE_SKIP_PROTOCOL_LOGGING detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_PROTOCOL_LOGGING not detected ... dragging in package")

   set (OPENSYDE_CORE_PROTOCOL_LOGGING_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolCANopen.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolGD.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolJ1939.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolKEFEX.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolL2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolOpenSYDE.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolSHIPIPIVA.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolSTWFF.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolXFL.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComLoggerProtocolOpenSYDE.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComLoggerProtocols.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLogger.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerFileAsc.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerFileBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerOsySysDefConfig.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocol.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolCANopen.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolGD.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolJ1939.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolKEFEX.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolL2.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolOpenSYDE.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolSHIPIPIVA.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolSTWFF.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolTarget.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CCMONProtocolXFL.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CDLReportEvents.h
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/CDLReportEvents.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComLoggerProtocolOpenSYDE.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComLoggerProtocols.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLogger.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerData.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerFileAsc.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerFileBase.h
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OSCComMessageLoggerOsySysDefConfig.h
   )
   set(OPENSYDE_CORE_PROTOCOL_LOGGING_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib
   )
endif()

# all sources
set(OPENSYDE_CORE_SOURCES
   ${OPENSYDE_CORE_COMMON_SOURCES}
   ${OPENSYDE_CORE_ZIPPING_SOURCES}
   ${OPENSYDE_CORE_IMPORTS_SOURCES}
   ${OPENSYDE_CORE_CODE_GENERATION_SOURCES}
   ${OPENSYDE_CORE_PROJECT_HANDLING_SOURCES}
   ${OPENSYDE_CORE_PARAM_SET_SOURCES}
   ${OPENSYDE_CORE_PROTOCOL_DRIVERS_SOURCES}
   ${OPENSYDE_CORE_WINDOWS_DRIVERS_SOURCES}
   ${OPENSYDE_CORE_WINDOWS_TARGET_SOURCES}
   ${OPENSYDE_CORE_PROTOCOL_LOGGING_SOURCES}
)

# all include directories
set(OPENSYDE_CORE_INCLUDE_DIRECTORIES
   ${OPENSYDE_CORE_COMMON_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_ZIPPING_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_IMPORTS_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_CODE_GENERATION_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_PROJECT_HANDLING_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_PARAM_SET_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_PROTOCOL_DRIVERS_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_WINDOWS_DRIVERS_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_WINDOWS_TARGET_INCLUDE_DIRECTORIES}
   ${OPENSYDE_CORE_PROTOCOL_LOGGING_INCLUDE_DIRECTORIES}
)

# define library target including all sources
add_library(opensyde_core ${OPENSYDE_CORE_SOURCES})

# add all include directories to library target and make public
target_include_directories(opensyde_core PUBLIC ${OPENSYDE_CORE_INCLUDE_DIRECTORIES})
