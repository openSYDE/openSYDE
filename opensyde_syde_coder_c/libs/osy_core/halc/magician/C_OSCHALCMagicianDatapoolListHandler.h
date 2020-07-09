//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class to simplify HALC datapool list access
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCMAGICIANDATAPOOLLISTHANDLER_H
#define C_OSCHALCMAGICIANDATAPOOLLISTHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCNodeDataPoolList.h"
#include "C_OSCHalcDefBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHALCMagicianDatapoolListHandler
{
public:
   C_OSCHALCMagicianDatapoolListHandler(const C_OSCHalcDefBase & orc_HalcDef);

   C_OSCNodeDataPoolListElement * GetListElement(const stw_types::uint32 ou32_DomainIndex,
                                                 const stw_types::uint32 ou32_ParameterStructIndex,
                                                 const stw_types::uint32 ou32_ParameterStructElementIndex,
                                                 C_OSCNodeDataPoolList & orc_List,
                                                 const C_OSCHalcDefDomain::E_VariableSelector oe_Selector) const;
   const C_OSCNodeDataPoolListElement * GetListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                            const stw_types::uint32 ou32_ParameterStructIndex,
                                                            const stw_types::uint32 ou32_ParameterStructElementIndex,
                                                            const C_OSCNodeDataPoolList & orc_List,
                                                            const C_OSCHalcDefDomain::E_VariableSelector oe_Selector)
   const;
   C_OSCNodeDataPoolListElement * GetUseCaseListElement(const stw_types::uint32 ou32_DomainIndex,
                                                        C_OSCNodeDataPoolList & orc_List) const;
   const C_OSCNodeDataPoolListElement * GetUseCaseListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                                   const C_OSCNodeDataPoolList & orc_List)
   const;

private:
   //lint -e{1725} it is indeed a reference (improves performance)
   const C_OSCHalcDefBase & mrc_HalcDef;

   stw_types::sint32 m_GetListIndex(const stw_types::uint32 ou32_DomainIndex,
                                    const stw_types::uint32 ou32_ParameterStructIndex,
                                    const stw_types::uint32 ou32_ParameterStructElementIndex,
                                    stw_types::uint32 & oru32_ListIndex,
                                    const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                    const bool oq_GetUseCaseIndex) const;

   static stw_types::uint32 mh_CountElements(const std::vector<C_OSCHalcDefStruct> & orc_Structs);
   static stw_types::uint32 mh_CountElements(const C_OSCHalcDefStruct & orc_Struct);

   //Avoid call
   C_OSCHALCMagicianDatapoolListHandler(const C_OSCHALCMagicianDatapoolListHandler &);
   C_OSCHALCMagicianDatapoolListHandler & operator =(const C_OSCHALCMagicianDatapoolListHandler &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
