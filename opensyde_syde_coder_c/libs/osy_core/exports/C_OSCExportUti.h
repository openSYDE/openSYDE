//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for C code export.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTUTI_H
#define C_OSCEXPORTUTI_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLStringList.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportUti
{
public:
   C_OSCExportUti();

   static stw_scl::C_SCLString h_GetSectionSeparator(const stw_scl::C_SCLString & orc_SectionName);
   static stw_scl::C_SCLString h_GetHeaderSeparator(void);
   static stw_scl::C_SCLString h_GetCreationToolInfo(const stw_scl::C_SCLString & orc_ExportToolInfo);
   static void h_AddExternCStart(stw_scl::C_SCLStringList & orc_Data);
   static void h_AddExternCEnd(stw_scl::C_SCLStringList & orc_Data);
   static void h_AddProjectIdDef(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_MagicName,
                                 const bool oq_HeaderFile);
   static void h_AddProjIdFunctionPrototype(stw_scl::C_SCLStringList & orc_Data,
                                            const stw_scl::C_SCLString & orc_MagicName);
   static stw_types::sint32 h_SaveToFile(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_Path,
                                         const stw_scl::C_SCLString & orc_FileName, const bool oq_HeaderFile);
   static void h_CollectFilePaths(std::vector<stw_scl::C_SCLString> & orc_FilePaths,
                                  const stw_scl::C_SCLString & orc_Path, const stw_scl::C_SCLString & orc_FileName);

   static stw_scl::C_SCLString h_GetTypePrefix(const C_OSCNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray);
   static stw_scl::C_SCLString h_GetElementCName(const stw_scl::C_SCLString & orc_Name, const bool oq_IsArray,
                                                 const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                 const stw_scl::C_SCLString & orc_ArrayPos = "0");

   static stw_scl::C_SCLString h_FloatToStrCutZeroes(const stw_types::float64 of64_Value,
                                                     const bool oq_MakePrecise = false);
   static stw_scl::C_SCLString h_FloatToStrCutZeroes(const stw_types::float32 of32_Value,
                                                     const bool oq_MakePrecise = false);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
