# This file lists all openSYDE Core files and include paths.
# It can be included from the CMakeLists.txt file of an application.
# Some optional parts of the core can be skipped by setting options in the application CMakeLists.txt file before
#  including this file in order to keep build times low.
# See the "OPENSYDE_CORE_SKIP_*" checks below for parts that can be skipped
#
# This CMakeLists.txt defined the library "opensyde_core" to be linked from the application.
# As the openSYDE core needs the "precomp_headers.hpp" that is placed with the application the path
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
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OscBinaryHash.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OscChecksummedIniFile.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OscUtils.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_string_resources/C_SclResourceStrings.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_string_resources/DLStrings.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/stwcompid/stwcompid.c
   ${CMAKE_CURRENT_SOURCE_DIR}/logging/C_OscLoggingHandler.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/md5/C_Md5Checksum.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclChecksums.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclDateTime.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclIniFile.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclString.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclStringList.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OscXmlParser.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OscXmlParserLog.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OscChecksummedXml.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/tinyxml2/tinyxml2.cpp

   ${CMAKE_CURRENT_SOURCE_DIR}/C_OscBinaryHash.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OscChecksummedIniFile.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/C_OscUtils.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_string_resources/C_SclResourceStrings.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_string_resources/DLStrings.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/stwcompid/stwcompid.h
   ${CMAKE_CURRENT_SOURCE_DIR}/logging/C_OscLoggingHandler.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/md5/C_Md5Checksum.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclChecksums.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclDateTime.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclIniFile.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclString.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclStringList.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/scl/C_SclDynamicArray.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/stwerrors.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/stwerrors.h
   ${CMAKE_CURRENT_SOURCE_DIR}/stwtypes/stwtypes.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/stwtypes/stwtypes.h
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OscXmlParser.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OscXmlParserLog.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/C_OscChecksummedXml.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser/tinyxml2/tinyxml2.h
)

set(OPENSYDE_CORE_COMMON_INCLUDE_DIRECTORIES
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_string_resources
   ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/stwcompid
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
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OscAesFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OscZipData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OscZipFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/aes/AES.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/miniz/miniz.c
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OscAesFile.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OscZipData.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/C_OscZipFile.hpp
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
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscImportEdsDcf.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscImportEdsDcf.hpp
   )
endif()

# source code generation
if(OPENSYDE_CORE_SKIP_CODE_GENERATION)
   message("OPENSYDE_CORE_SKIP_CODE_GENERATION detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_CODE_GENERATION not detected ... dragging in package")

   set(OPENSYDE_CORE_CODE_GENERATION_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportCommunicationStack.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportCanOpenConfig.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportCanOpenInit.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportDataPool.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportHalc.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportOsyInit.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportParamSet.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportUti.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportCommunicationStack.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportCanOpenConfig.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportCanOpenInit.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportDataPool.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportHalc.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportNode.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportOsyInit.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportParamSet.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/exports/C_OscExportUti.hpp
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
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfig.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigChannel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigDomain.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigParameter.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigParameterStruct.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandalone.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandaloneChannel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandaloneDomain.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandaloneFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/base/C_OscHalcDefBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDef.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelAvailability.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelDef.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelUseCase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelValues.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefContent.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefContentBitmaskItem.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefDomain.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefElement.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefStruct.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefStructFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OscHalcMagicianDatapoolListHandler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OscHalcMagicianGenerator.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OscHalcMagicianUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OscProject.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OscProjectFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceDefinitionFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceDefinitionFilerV1.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceDefinition.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSubDeviceDefinition.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceGroup.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceManager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemBus.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemBusFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemDefinition.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemDefinitionFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemFilerUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscTargetSupportPackage.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscTargetSupportPackageFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscNodeDataPoolFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscNodeFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscSystemBusFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscSystemDefinitionFilerV2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessage.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessageUniqueId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessageContainer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessageIdentificationIndices.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanSignal.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanInterfaceId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerMappableSignal.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerDeviceInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeSquad.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeSquadFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeApplication.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeCodeExportSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeComInterfaceSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPool.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolContent.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolContentUtil.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolDataSet.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolList.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolListElement.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolListElementId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolListId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeOpenSydeServerSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeProperties.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeStwFlashloaderSettings.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeCommFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolId.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewPc.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewNodeUpdate.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewNodeUpdateParamInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenObjectDictionary.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenEdsInfoBlock.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenEdsDeviceInfoBlock.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenEdsFileInfoBlock.cpp

      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfig.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigChannel.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigDomain.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigParameter.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigParameterStruct.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/C_OscHalcConfigUtil.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandalone.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandaloneChannel.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandaloneDomain.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/configuration/standalone/C_OscHalcConfigStandaloneFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/base/C_OscHalcDefBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDef.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelAvailability.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelDef.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelUseCase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefChannelValues.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefContent.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefContentBitmaskItem.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefDomain.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefElement.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefStruct.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/definition/C_OscHalcDefStructFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OscHalcMagicianDatapoolListHandler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OscHalcMagicianGenerator.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/halc/magician/C_OscHalcMagicianUtil.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OscProject.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/C_OscProjectFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceDefinition.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSubDeviceDefinition.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceDefinitionFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceDefinitionFilerV1.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceGroup.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscDeviceManager.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemBus.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemBusFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemDefinition.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemDefinitionFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscSystemFilerUtil.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscTargetSupportPackage.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/C_OscTargetSupportPackageFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscNodeDataPoolFilerV2.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscNodeFilerV2.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscSystemBusFilerV2.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/FileLoadersV2/C_OscSystemDefinitionFilerV2.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNode.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeSquad.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeSquadFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeApplication.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeCodeExportSettings.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolContentUtil.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolId.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeComInterfaceSettings.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeCommFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPool.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolContent.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolDataSet.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolList.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolListElement.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolListElementId.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeDataPoolListId.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeOpenSydeServerSettings.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeProperties.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/C_OscNodeStwFlashloaderSettings.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessage.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessageUniqueId.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessageContainer.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanMessageIdentificationIndices.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanProtocol.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanSignal.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanUtil.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/C_OscCanInterfaceId.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerMappableSignal.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerDeviceInfo.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerInfo.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open/C_OscCanOpenManagerFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewData.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewPc.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewNodeUpdate.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewNodeUpdateParamInfo.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view/C_OscViewFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenObjectDictionary.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenEdsInfoBlock.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenEdsDeviceInfoBlock.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imports/C_OscCanOpenEdsFileInfoBlock.hpp
   )

   set(OPENSYDE_CORE_PROJECT_HANDLING_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/imports
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
      ${CMAKE_CURRENT_SOURCE_DIR}/project/system/node/can/can_open
      ${CMAKE_CURRENT_SOURCE_DIR}/project/view
   )
endif()

# param set handling (reading and writing syde_psi files)
if(OPENSYDE_CORE_SKIP_PARAM_SET)
   message("OPENSYDE_CORE_SKIP_PARAM_SET detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_PARAM_SET not detected ... dragging in package")

   set(OPENSYDE_CORE_PARAM_SET_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetDataPoolInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetFilerBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetHandler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedDataPool.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedElement.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedFileInfoData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedList.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedNodeFiler.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetRawEntry.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetRawNode.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetRawNodeFiler.cpp

      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetDataPoolInfo.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetFilerBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetHandler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedData.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedDataPool.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedElement.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedFileInfoData.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedList.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedNode.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetInterpretedNodeFiler.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetRawEntry.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetRawNode.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/paramset/C_OscParamSetRawNodeFiler.hpp
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
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/C_CanBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/C_CanDispatcher.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OscDataDealer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OscDataDealerNvm.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OscDataDealerNvmSafe.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_pool/CDLReportEvents.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/hexfile/C_HexFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXComm.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXCommConfiguration.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXCommunicationKEFEX.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLActions.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLECUInformation.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLFlashWrite.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLFlashWriteParameters.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLHexFile.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityPem.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityPemKeyInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityPemDatabase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityRsa.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscCanDispatcherOsyRouter.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscDiagProtocolBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscDiagProtocolKfx.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscDiagProtocolOsy.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscFlashProtocolStwFlashloader.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolSerialNumber.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsy.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsyTpBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsyTpCan.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsyTpIp.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComDriverBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComDriverFlash.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComDriverProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComSequencesBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OscRoutingCalculation.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OscRoutingRoute.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OscSuSequences.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OscSuSequencesNodeStates.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OscSuServiceUpdatePackage.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OsyHexFile.cpp

      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/C_CanBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/C_CanDispatcher.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher/stw_can.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OscDataDealer.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OscDataDealerNvm.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer/C_OscDataDealerNvmSafe.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/dispatcher/C_OscIpDispatcher.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_pool/CDLReportEvents.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/hexfile/C_HexFile.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXComm.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXCommConfiguration.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXCommunicationKEFEX.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex/CKFXProtocol.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLActions.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLECUInformation.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLFlashWrite.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLFlashWriteParameters.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLHexFile.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash/CXFLProtocol.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityPem.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityPemKeyInfo.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityPemDatabase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/security/C_OscSecurityRsa.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscCanDispatcherOsyRouter.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscDiagProtocolBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscDiagProtocolKfx.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscDiagProtocolOsy.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscFlashProtocolStwFlashloader.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolSerialNumber.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsy.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsyTpBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsyTpCan.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/C_OscProtocolDriverOsyTpIp.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComDriverBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComDriverFlash.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComDriverProtocol.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComSequencesBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OscRoutingCalculation.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing/C_OscRoutingRoute.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OscSuSequences.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OscSuSequencesNodeStates.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OscSuServiceUpdatePackage.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update/C_OsyHexFile.hpp
   )

   set(OPENSYDE_CORE_PROTOCOL_DRIVERS_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/dispatcher
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/dispatcher
      ${CMAKE_CURRENT_SOURCE_DIR}/data_dealer
      ${CMAKE_CURRENT_SOURCE_DIR}/security
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/routing
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/system_update
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_pool
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/hexfile
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_kefex
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/dl_stwflash
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol
   )
endif()

# windows target drivers: CAN and Ethernet dispatchers
if(OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS)
   message("OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS detected ... skipping package")
else()
   message("OPENSYDE_CORE_SKIP_WINDOWS_DRIVERS not detected ... dragging in package")

   set (OPENSYDE_CORE_WINDOWS_DRIVERS_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/C_Can.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/C_CanDll.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/target_windows_win_sock/C_OscIpDispatcherWinSock.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/C_Can.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/can_dispatcher/target_windows_stw_dlls/C_CanDll.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/ip_dispatcher/target_windows_win_sock/C_OscIpDispatcherWinSock.hpp
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
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLFile.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLTasks.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLTime.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/tgl_windows/TGLUtils.hpp
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
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocol.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolCanOpen.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolGd.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolJ1939.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolKefex.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolL2.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolOpenSyde.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolShipIpIva.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolStwFf.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolXfl.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComLoggerProtocolOpenSyde.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComLoggerProtocols.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLogger.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerFileAsc.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerFileBase.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerOsySysDefConfig.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocol.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolCanOpen.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolGd.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolJ1939.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolKefex.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolL2.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolOpenSyde.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolShipIpIva.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolStwFf.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol/C_CanMonProtocolXfl.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/C_CanMonProtocolTarget.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComLoggerProtocolOpenSyde.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComLoggerProtocols.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLogger.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerData.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerFileAsc.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerFileBase.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/protocol_drivers/communication/C_OscComMessageLoggerOsySysDefConfig.hpp
   )
   set(OPENSYDE_CORE_PROTOCOL_LOGGING_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib
      ${CMAKE_CURRENT_SOURCE_DIR}/kefex_diaglib/cmonprotocol
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
