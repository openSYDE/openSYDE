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
#include "C_OSCHalcConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHALCMagicianDatapoolListHandler
{
public:
   C_OSCHALCMagicianDatapoolListHandler(const C_OSCHalcConfig & orc_HalcConfig);

   C_OSCNodeDataPoolListElement * GetListElement(const stw_types::uint32 ou32_DomainIndex,
                                                 const stw_types::uint32 ou32_ParameterStructIndex,
                                                 const stw_types::uint32 ou32_ParameterStructElementIndex,
                                                 C_OSCNodeDataPoolList & orc_List,
                                                 const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                                 const bool oq_IsSafe) const;
   const C_OSCNodeDataPoolListElement * GetListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                            const stw_types::uint32 ou32_ParameterStructIndex,
                                                            const stw_types::uint32 ou32_ParameterStructElementIndex,
                                                            const C_OSCNodeDataPoolList & orc_List,
                                                            const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                                            const bool oq_IsSafe)
   const;
   C_OSCNodeDataPoolListElement * GetUseCaseListElement(const stw_types::uint32 ou32_DomainIndex,
                                                        C_OSCNodeDataPoolList & orc_List, const bool oq_IsSafe) const;
   const C_OSCNodeDataPoolListElement * GetUseCaseListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                                   const C_OSCNodeDataPoolList & orc_List,
                                                                   const bool oq_IsSafe)
   const;
   C_OSCNodeDataPoolListElement * GetChanNumListElement(const stw_types::uint32 ou32_DomainIndex,
                                                        C_OSCNodeDataPoolList & orc_List, const bool oq_IsSafe) const;
   const C_OSCNodeDataPoolListElement * GetChanNumListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                                   const C_OSCNodeDataPoolList & orc_List,
                                                                   const bool oq_IsSafe)
   const;

   static stw_types::uint32 h_CountRelevantItems(const std::vector<C_OSCHalcConfigChannel> & orc_Channels,
                                                 const C_OSCHalcConfigChannel & orc_DomainConfig, const bool oq_IsSafe);

private:
   //lint -e{1725} it is indeed a reference (improves performance)
   const C_OSCHalcConfig & mrc_HalcConfig;

   stw_types::sint32 m_GetListIndex(const stw_types::uint32 ou32_DomainIndex,
                                    const stw_types::uint32 ou32_ParameterStructIndex,
                                    const stw_types::uint32 ou32_ParameterStructElementIndex,
                                    stw_types::uint32 & oru32_ListIndex,
                                    const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                    const bool oq_GetUseCaseIndex, const bool oq_GetChanNumIndex,
                                    const bool oq_IsSafe) const;

   static stw_types::uint32 mh_CountElements(const std::vector<C_OSCHalcDefStruct> & orc_Structs);
   static stw_types::uint32 mh_CountElements(const C_OSCHalcDefStruct & orc_Struct);
   static stw_types::sint32 mh_GetSubElementIndex(const stw_types::uint32 ou32_Index,
                                                  const stw_types::uint32 ou32_ElementIndex,
                                                  const std::vector<C_OSCHalcDefStruct> & orc_Values,
                                                  stw_types::uint32 & oru32_ListIndex);

   //Avoid call
   C_OSCHALCMagicianDatapoolListHandler(const C_OSCHALCMagicianDatapoolListHandler &);
   C_OSCHALCMagicianDatapoolListHandler & operator =(const C_OSCHALCMagicianDatapoolListHandler &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
