//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export HALC configuration of an openSYDE node.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTHALC_H
#define C_OSCEXPORTHALC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLStringList.h"
#include "C_OSCHalcConfig.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportHalc
{
public:
   C_OSCExportHalc(void);

   static stw_scl::C_SCLString h_GetFileName(const bool oq_IsSafe);
   static stw_types::uint16 h_ConvertOverallCodeVersion(const stw_types::uint16 ou16_GenCodeVersion);
   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_Path,
                                               const C_OSCHalcConfig & orc_HalcConfig,
                                               const stw_opensyde_core::C_OSCNodeDataPool & orc_Datapool,
                                               const stw_scl::C_SCLString & orc_ExportToolInfo);

protected:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

private:
   static stw_types::sint32 mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                const stw_scl::C_SCLString & orc_Path,
                                                const C_OSCHalcConfig & orc_HalcConfig,
                                                const stw_scl::C_SCLString & orc_ProjectId, const bool oq_IsSafe);

   static stw_types::sint32 mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                        const stw_scl::C_SCLString & orc_Path,
                                                        const C_OSCHalcConfig & orc_HalcConfig,
                                                        const stw_scl::C_SCLString & orc_ProjectId,
                                                        const bool oq_IsSafe);

   static void mh_AddHeader(const stw_scl::C_SCLString & orc_ExportToolInfo, stw_scl::C_SCLStringList & orc_Data,
                            const bool oq_FileType, const bool oq_IsSafe);
   static void mh_AddIncludes(stw_scl::C_SCLStringList & orc_Data, const bool oq_FileType, const bool oq_IsSafe);
   static void mh_AddDefines(stw_scl::C_SCLStringList & orc_Data,  const C_OSCHalcConfig & orc_HalcConfig,
                             const stw_scl::C_SCLString & orc_ProjectId, const bool oq_FileType, const bool oq_IsSafe);
   static void mh_AddGlobalVariables(stw_scl::C_SCLStringList & orc_Data, const C_OSCHalcConfig & orc_HalcConfig,
                                     const bool oq_FileType, const bool oq_IsSafe);
   static stw_scl::C_SCLString mh_GetMagicName(const stw_scl::C_SCLString & orc_ProjectId, const bool oq_IsSafe);
   static void mh_AddDpListElementReferences(stw_scl::C_SCLStringList & orc_Data,
                                             const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                             const std::vector<C_OSCHalcConfigChannel> & orc_ConfigArray,
                                             const C_OSCHalcDefDomain::E_VariableSelector & ore_Type,
                                             const stw_scl::C_SCLString & orc_DomainSingularName, const bool oq_IsArray,
                                             const bool oq_IsSafe);
   static stw_scl::C_SCLString mh_GetDpListElementReference(const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                                            const C_OSCHalcDefDomain::E_VariableSelector & ore_Type,
                                                            const C_OSCHalcDefElement & orc_Element,
                                                            const stw_types::uint32 ou32_StructIndex,
                                                            const stw_types::uint32 ou32_ElementIndex,
                                                            const stw_scl::C_SCLString & orc_DomainSingularName,
                                                            const bool oq_IsArray, const bool oq_IsSafe);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
