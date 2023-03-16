//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export Datapool settings of an openSYDE node.

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTDATAPOOLH
#define C_OSCEXPORTDATAPOOLH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"

#include "C_SclStringList.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_OscNodeCodeExportSettings.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscExportDataPool
{
public:
   enum E_Linkage ///< Flag for Datapool-application-relation when generating source code files
   {
      eLOCAL = 0,   ///< Generate code for Datapool owner
      eREMOTE,      ///< Generate code for DPD owner
      eREMOTEPUBLIC ///< Generate code for public Datapool that is owned by another application
   };

   static stw::scl::C_SclString h_GetFileName(const C_OscNodeDataPool & orc_DataPool);
   static uint16_t h_ConvertOverallCodeVersion(const uint16_t ou16_GenCodeVersion);
   static int32_t h_CreateSourceCode(const stw::scl::C_SclString & orc_Path, const uint16_t ou16_GenCodeVersion,
                                     const C_OscNodeCodeExportSettings::E_Scaling oe_ScalingSupport,
                                     const C_OscNodeDataPool & orc_DataPool, const uint8_t ou8_DataPoolIndex,
                                     const E_Linkage oe_Linkage, const uint8_t ou8_DataPoolIndexRemote,
                                     const uint8_t ou8_ProcessId,
                                     const stw::scl::C_SclString & orc_ExportToolInfo = "");

private:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static int32_t mh_CreateImplementationFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                              const stw::scl::C_SclString & orc_Path,
                                              const C_OscNodeDataPool & orc_DataPool,
                                              const stw::scl::C_SclString & orc_ProjectId,
                                              const uint16_t ou16_GenCodeVersion, const uint8_t ou8_DataPoolIndexRemote,
                                              const uint8_t ou8_ProcessId, const E_Linkage oe_Linkage);
   static int32_t mh_CreateHeaderFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                      const stw::scl::C_SclString & orc_Path, const C_OscNodeDataPool & orc_DataPool,
                                      const uint8_t ou8_DataPoolIndex, const stw::scl::C_SclString & orc_ProjectId,
                                      const uint16_t ou16_GenCodeVersion, const E_Linkage oe_Linkage,
                                      const C_OscNodeCodeExportSettings::E_Scaling oe_ScalingSupport);

   static void mh_AddHeader(const stw::scl::C_SclString & orc_ExportToolInfo, stw::scl::C_SclStringList & orc_Data,
                            const C_OscNodeDataPool & orc_DataPool, const bool oq_FileType);
   static void mh_AddIncludes(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                              const bool oq_FileType);
   static int32_t mh_AddDefinesHeader(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                                      const uint8_t ou8_DataPoolIndex, const stw::scl::C_SclString & orc_ProjectId,
                                      const uint16_t ou16_GenCodeVersion, const E_Linkage oe_Linkage,
                                      const C_OscNodeCodeExportSettings::E_Scaling oe_ScalingSupport);
   static void mh_AddDefinesImpl(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                                 const stw::scl::C_SclString & orc_ProjectId, const uint16_t ou16_GenCodeVersion);
   static void mh_AddTypes(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                           const bool oq_FileType, const E_Linkage oe_Linkage);
   static void mh_AddGlobalVariables(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                                     const uint16_t ou16_GenCodeVersion, const bool oq_FileType,
                                     const E_Linkage oe_Linkage);
   static void mh_AddModuleGlobal(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                                  const uint16_t ou16_GenCodeVersion, const uint8_t ou8_ProcessId,
                                  const uint8_t ou8_DataPoolIndexRemote, const E_Linkage oe_Linkage);
   static void mh_AddImplementation(stw::scl::C_SclStringList & orc_Data, const bool oq_FileType);
   static stw::scl::C_SclString mh_GetType(const C_OscNodeDataPoolContent::E_Type oe_Type);
   static stw::scl::C_SclString mh_GetElementValueString(const C_OscNodeDataPoolContent & orc_Value,
                                                         const C_OscNodeDataPoolContent::E_Type oe_Type,
                                                         const bool oq_IsArray);
   static stw::scl::C_SclString mh_GetElementSize(const C_OscNodeDataPoolContent::E_Type oe_Type,
                                                  const uint32_t ou32_ArraySize, const bool oq_IsArray);
   static stw::scl::C_SclString mh_ConvertLinkageToString(const E_Linkage oe_Linkage);
   static stw::scl::C_SclString mh_GetMagicName(const stw::scl::C_SclString & orc_ProjectId,
                                                const C_OscNodeDataPool & orc_DataPool);
   static stw::scl::C_SclString mh_GetElementScaleDefine(const stw::scl::C_SclString & orc_DataPoolName,
                                                         const stw::scl::C_SclString & orc_ListName,
                                                         const stw::scl::C_SclString & orc_ElementName,
                                                         const bool oq_Factor);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
