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

private:
   static stw_scl::C_SCLString mh_GetElementName(const C_OSCHalcDefElement & orc_Param,
                                                 const stw_scl::C_SCLString & orc_Domain);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
