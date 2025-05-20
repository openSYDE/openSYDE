//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export HALC configuration of an openSYDE node.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTHALC_HPP
#define C_OSCEXPORTHALC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclStringList.hpp"
#include "C_OscHalcConfig.hpp"
#include "C_OscNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscExportHalc
{
public:
   static stw::scl::C_SclString h_GetFileName(const bool oq_IsSafe);
   static uint16_t h_ConvertOverallCodeVersion(const uint16_t ou16_GenCodeVersion);
   static int32_t h_CreateSourceCode(const stw::scl::C_SclString & orc_Path, const uint16_t ou16_GenCodeVersion,
                                     const C_OscHalcConfig & orc_HalcConfig,
                                     const stw::opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                     const stw::scl::C_SclString & orc_ExportToolInfo);

protected:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static int32_t mh_CreateHeaderFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                      const stw::scl::C_SclString & orc_Path, const C_OscHalcConfig & orc_HalcConfig,
                                      const stw::scl::C_SclString & orc_ProjectId, const bool oq_IsSafe,
                                      const uint16_t ou16_GenCodeVersion);

   static int32_t mh_CreateImplementationFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                              const stw::scl::C_SclString & orc_Path,
                                              const C_OscHalcConfig & orc_HalcConfig,
                                              const stw::scl::C_SclString & orc_ProjectId, const bool oq_IsSafe,
                                              const uint16_t ou16_GenCodeVersion);

   static void mh_AddHeader(const stw::scl::C_SclString & orc_ExportToolInfo, stw::scl::C_SclStringList & orc_Data,
                            const bool oq_FileType, const bool oq_IsSafe);
   static void mh_AddIncludes(stw::scl::C_SclStringList & orc_Data, const bool oq_FileType, const bool oq_IsSafe);
   static void mh_AddDefines(stw::scl::C_SclStringList & orc_Data,  const C_OscHalcConfig & orc_HalcConfig,
                             const stw::scl::C_SclString & orc_ProjectId, const bool oq_FileType, const bool oq_IsSafe,
                             const uint16_t ou16_GenCodeVersion);
   static void mh_AddGlobalVariables(stw::scl::C_SclStringList & orc_Data, const C_OscHalcConfig & orc_HalcConfig,
                                     const bool oq_FileType, const bool oq_IsSafe);
   static stw::scl::C_SclString mh_GetMagicName(const stw::scl::C_SclString & orc_ProjectId, const bool oq_IsSafe);
   static void mh_AddDpListElementReferences(stw::scl::C_SclStringList & orc_Data,
                                             const std::vector<C_OscHalcDefStruct> & orc_DefinitionArray,
                                             const std::vector<C_OscHalcConfigChannel> & orc_ConfigArray,
                                             const C_OscHalcDefDomain::E_VariableSelector & ore_Type,
                                             const stw::scl::C_SclString & orc_DomainSingularName,
                                             const bool oq_IsArray, const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode,
                                             const bool oq_IsSafe);
   static stw::scl::C_SclString mh_GetDpListElementReference(
      const std::vector<C_OscHalcDefStruct> & orc_DefinitionArray,
      const C_OscHalcDefDomain::E_VariableSelector & ore_Type, const C_OscHalcDefElement & orc_Element,
      const uint32_t ou32_StructIndex, const uint32_t ou32_ElementIndex,
      const stw::scl::C_SclString & orc_DomainSingularName, const bool oq_IsArray, const bool oq_IsSafe);
   static bool mh_DropChannel(const bool oq_IsChannelSafe, const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode,
                              const bool oq_IsSafeCase);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
