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
#include "C_OSCHalcConfigDomain.h"
#include "C_OSCNodeDataPoolListElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHALCMagicianUtil
{
public:
   C_OSCHALCMagicianUtil(void);

   static stw_scl::C_SCLString h_GetDatapoolName(const bool oq_IsSafe);
   static stw_scl::C_SCLString h_GetListName(const C_OSCHalcDefDomain::E_VariableSelector oe_Type);
   static stw_types::sint32 h_GetVariableName(const std::vector<C_OSCHalcDefStruct> & orc_DefinitionArray,
                                              const stw_types::uint32 ou32_ParameterIndexStruct,
                                              const stw_types::uint32 ou32_ParameterIndexElement,
                                              const stw_scl::C_SCLString & orc_DomainSingularName,
                                              stw_scl::C_SCLString & orc_Name);
   static stw_scl::C_SCLString h_CombineVariableName(const stw_scl::C_SCLString & orc_Domain,
                                                     const stw_scl::C_SCLString & orc_ParameterName);
   static C_OSCNodeDataPoolListElement h_GetChanNumVariable(const stw_scl::C_SCLString & orc_DomainSingularName,
                                                            const bool oq_IsSafe,
                                                            const stw_types::uint32 ou32_NumChannels,
                                                            const bool oq_AddDataset);
   static stw_scl::C_SCLString h_GetChanNumVariableName(const stw_scl::C_SCLString & orc_DomainSingularName);
   static C_OSCNodeDataPoolListElement h_GetUseCaseVariable(const stw_scl::C_SCLString & orc_DomainSingularName,
                                                            const bool oq_IsSafe,
                                                            const stw_types::uint32 ou32_NumChannels,
                                                            const bool oq_AddDataset);
   static stw_scl::C_SCLString h_GetUseCaseVariableName(const stw_scl::C_SCLString & orc_DomainSingularName);
   static void h_SetCommonDpElementDefaults(C_OSCNodeDataPoolListElement & orc_Element);
   static bool h_CheckChanNumVariableNecessary(const C_OSCHalcConfigDomain & orc_Domain);
   static bool h_CheckUseCaseVariableNecessary(const C_OSCHalcConfigDomain & orc_Domain);

private:
   static stw_scl::C_SCLString mh_GetElementName(const C_OSCHalcDefElement & orc_Param,
                                                 const stw_scl::C_SCLString & orc_Domain);
   static void mh_HandleGenericType(C_OSCNodeDataPoolListElement & orc_Element,
                                    const stw_types::uint32 ou32_NumChannels, const bool oq_AddDataset,
                                    const bool oq_UseU16);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
