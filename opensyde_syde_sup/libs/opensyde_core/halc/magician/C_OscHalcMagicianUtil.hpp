//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for HALC magician related functionality
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCMAGICIANUTIL_H
#define C_OSCHALCMAGICIANUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscHalcConfigDomain.hpp"
#include "C_OscNodeDataPoolListElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcMagicianUtil
{
public:
   C_OscHalcMagicianUtil(void);

   static stw::scl::C_SclString h_GetDatapoolName(const bool oq_IsSafe, const uint32_t ou32_CopyIndex = 0UL);
   static stw::scl::C_SclString h_GetDatapoolComment(const bool oq_IsSafe, const uint32_t ou32_CopyIndex = 0UL);
   static stw::scl::C_SclString h_GetListName(const C_OscHalcDefDomain::E_VariableSelector oe_Type);
   static int32_t h_GetVariableName(const std::vector<C_OscHalcDefStruct> & orc_DefinitionArray,
                                    const uint32_t ou32_ParameterIndexStruct, const uint32_t ou32_ParameterIndexElement,
                                    const stw::scl::C_SclString & orc_DomainSingularName,
                                    stw::scl::C_SclString & orc_Name);
   static stw::scl::C_SclString h_CombineVariableName(const stw::scl::C_SclString & orc_Domain,
                                                      const stw::scl::C_SclString & orc_ParameterName);
   static C_OscNodeDataPoolListElement h_GetChanNumVariable(const stw::scl::C_SclString & orc_DomainSingularName,
                                                            const bool oq_IsSafe, const uint32_t ou32_NumChannels,
                                                            const bool oq_AddDataset);
   static stw::scl::C_SclString h_GetChanNumVariableName(const stw::scl::C_SclString & orc_DomainSingularName);
   static C_OscNodeDataPoolListElement h_GetUseCaseVariable(const stw::scl::C_SclString & orc_DomainSingularName,
                                                            const bool oq_IsSafe, const uint32_t ou32_NumChannels,
                                                            const bool oq_AddDataset);
   static stw::scl::C_SclString h_GetUseCaseVariableName(const stw::scl::C_SclString & orc_DomainSingularName);
   static C_OscNodeDataPoolListElement h_GetSafetyFlagVariable(const stw::scl::C_SclString & orc_DomainSingularName,
                                                               const bool oq_IsSafe, const uint32_t ou32_NumChannels,
                                                               const bool oq_AddDataset);
   static stw::scl::C_SclString h_GetSafetyFlagVariableName(const stw::scl::C_SclString & orc_DomainSingularName);
   static void h_SetCommonDpElementDefaults(C_OscNodeDataPoolListElement & orc_Element);

private:
   static stw::scl::C_SclString mh_GetElementName(const C_OscHalcDefElement & orc_Param,
                                                  const stw::scl::C_SclString & orc_Domain);
   static void mh_HandleGenericType(C_OscNodeDataPoolListElement & orc_Element, const uint32_t ou32_NumChannels,
                                    const bool oq_AddDataset, const bool oq_UseU16);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
