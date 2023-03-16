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
#include "C_SclStringList.hpp"
#include "C_OscNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscExportUti
{
public:
   static stw::scl::C_SclString h_GetSectionSeparator(const stw::scl::C_SclString & orc_SectionName);
   static stw::scl::C_SclString h_GetHeaderSeparator(void);
   static stw::scl::C_SclString h_GetCreationToolInfo(const stw::scl::C_SclString & orc_ExportToolInfo);
   static void h_AddExternCeStart(stw::scl::C_SclStringList & orc_Data);
   static void h_AddExternCeEnd(stw::scl::C_SclStringList & orc_Data);
   static void h_AddProjectIdDef(stw::scl::C_SclStringList & orc_Data, const stw::scl::C_SclString & orc_MagicName,
                                 const bool oq_HeaderFile);
   static void h_AddProjIdFunctionPrototype(stw::scl::C_SclStringList & orc_Data,
                                            const stw::scl::C_SclString & orc_MagicName);
   static int32_t h_SaveToFile(stw::scl::C_SclStringList & orc_Data, const stw::scl::C_SclString & orc_Path,
                               const stw::scl::C_SclString & orc_FileName, const bool oq_HeaderFile);
   static void h_CollectFilePaths(std::vector<stw::scl::C_SclString> & orc_FilePaths,
                                  const stw::scl::C_SclString & orc_Path, const stw::scl::C_SclString & orc_FileName,
                                  const bool oq_SourceCode);

   static stw::scl::C_SclString h_GetTypePrefix(const C_OscNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray);
   static stw::scl::C_SclString h_GetElementTypeAsString(const C_OscNodeDataPoolContent::E_Type oe_Type);
   static stw::scl::C_SclString h_GetElementCeName(const stw::scl::C_SclString & orc_Name, const bool oq_IsArray,
                                                   const C_OscNodeDataPoolContent::E_Type oe_Type,
                                                   const stw::scl::C_SclString & orc_ArrayPos = "0");

   static stw::scl::C_SclString h_FloatToStrGe(const float32_t of32_Value,  bool * const opq_InfOrNan = NULL);
   static stw::scl::C_SclString h_FloatToStrGe(const float64_t of64_Value, bool * const opq_InfOrNan = NULL);
   static bool h_CheckInfOrNan(const stw::scl::C_SclString & orc_String);
   static void h_AddDecimalPointIfNone(stw::scl::C_SclString & orc_FloatString);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
