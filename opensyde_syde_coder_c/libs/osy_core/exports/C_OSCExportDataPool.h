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

#include "stwtypes.h"

#include "CSCLStringList.h"
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeCodeExportSettings.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportDataPool
{
public:
   enum E_Linkage ///< Flag for Datapool-application-relation when generating source code files
   {
      eLOCAL = 0,   ///< Generate code for Datapool owner
      eREMOTE,      ///< Generate code for DPD owner
      eREMOTEPUBLIC ///< Generate code for public Datapool that is owned by another application
   };

   static stw_scl::C_SCLString h_GetFileName(const C_OSCNodeDataPool & orc_DataPool);
   static stw_types::uint16 h_ConvertOverallCodeVersion(const stw_types::uint16 ou16_GenCodeVersion);
   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_Path,
                                               const stw_types::uint16 ou16_GenCodeVersion,
                                               const C_OSCNodeCodeExportSettings::E_Scaling oe_ScalingSupport,
                                               const C_OSCNodeDataPool & orc_DataPool,
                                               const stw_types::uint8 ou8_DataPoolIndex, const E_Linkage oe_Linkage,
                                               const stw_types::uint8 ou8_DataPoolIndexRemote,
                                               const stw_types::uint8 ou8_ProcessId,
                                               const stw_scl::C_SCLString & orc_ExportToolInfo = "");

private:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static stw_types::sint32 mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                        const stw_scl::C_SCLString & orc_Path,
                                                        const C_OSCNodeDataPool & orc_DataPool,
                                                        const stw_scl::C_SCLString & orc_ProjectId,
                                                        const stw_types::uint16 ou16_GenCodeVersion,
                                                        const stw_types::uint8 ou8_DataPoolIndexRemote,
                                                        const stw_types::uint8 ou8_ProcessId,
                                                        const E_Linkage oe_Linkage);
   static stw_types::sint32 mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                const stw_scl::C_SCLString & orc_Path,
                                                const C_OSCNodeDataPool & orc_DataPool,
                                                const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_scl::C_SCLString & orc_ProjectId,
                                                const stw_types::uint16 ou16_GenCodeVersion, const E_Linkage oe_Linkage,
                                                const C_OSCNodeCodeExportSettings::E_Scaling oe_ScalingSupport);

   static void mh_AddHeader(const stw_scl::C_SCLString & orc_ExportToolInfo, stw_scl::C_SCLStringList & orc_Data,
                            const C_OSCNodeDataPool & orc_DataPool, const bool oq_FileType);
   static void mh_AddIncludes(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                              const bool oq_FileType);
   static stw_types::sint32 mh_AddDefinesHeader(stw_scl::C_SCLStringList & orc_Data,
                                                const C_OSCNodeDataPool & orc_DataPool,
                                                const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_scl::C_SCLString & orc_ProjectId,
                                                const stw_types::uint16 ou16_GenCodeVersion, const E_Linkage oe_Linkage,
                                                const C_OSCNodeCodeExportSettings::E_Scaling oe_ScalingSupport);
   static void mh_AddDefinesImpl(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                 const stw_scl::C_SCLString & orc_ProjectId,
                                 const stw_types::uint16 ou16_GenCodeVersion);
   static void mh_AddTypes(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                           const bool oq_FileType, const E_Linkage oe_Linkage);
   static void mh_AddGlobalVariables(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                     const stw_types::uint16 ou16_GenCodeVersion, const bool oq_FileType,
                                     const E_Linkage oe_Linkage);
   static void mh_AddModuleGlobal(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                  const stw_types::uint16 ou16_GenCodeVersion, const stw_types::uint8 ou8_ProcessId,
                                  const stw_types::uint8 ou8_DataPoolIndexRemote, const E_Linkage oe_Linkage);
   static void mh_AddImplementation(stw_scl::C_SCLStringList & orc_Data, const bool oq_FileType);
   static stw_scl::C_SCLString mh_GetType(const C_OSCNodeDataPoolContent::E_Type oe_Type);
   static stw_scl::C_SCLString mh_GetElementValueString(const C_OSCNodeDataPoolContent & orc_Value,
                                                        const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                        const bool oq_IsArray);
   static stw_scl::C_SCLString mh_GetElementSize(const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                 const stw_types::uint32 ou32_ArraySize, const bool oq_IsArray);
   static stw_scl::C_SCLString mh_ConvertLinkageToString(const E_Linkage oe_Linkage);
   static stw_scl::C_SCLString mh_GetMagicName(const stw_scl::C_SCLString & orc_ProjectId,
                                               const C_OSCNodeDataPool & orc_DataPool);
   static stw_scl::C_SCLString mh_GetElementScaleDefine(const stw_scl::C_SCLString & orc_DataPoolName,
                                                        const stw_scl::C_SCLString & orc_ListName,
                                                        const stw_scl::C_SCLString & orc_ElementName,
                                                        const bool oq_Factor);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
