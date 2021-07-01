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
   C_OSCHALCMagicianDatapoolListHandler(const C_OSCHalcConfig & orc_HalcConfig,
                                        const C_OSCHalcDefDomain::E_VariableSelector oe_Selector, const bool oq_IsSafe);

   C_OSCNodeDataPoolListElement * GetListElement(const stw_types::uint32 ou32_DomainIndex,
                                                 const stw_types::uint32 ou32_ParameterStructIndex,
                                                 const stw_types::uint32 ou32_ParameterStructElementIndex,
                                                 C_OSCNodeDataPoolList & orc_List) const;
   const C_OSCNodeDataPoolListElement * GetListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                            const stw_types::uint32 ou32_ParameterStructIndex,
                                                            const stw_types::uint32 ou32_ParameterStructElementIndex,
                                                            const C_OSCNodeDataPoolList & orc_List)
   const;
   C_OSCNodeDataPoolListElement * GetUseCaseListElement(const stw_types::uint32 ou32_DomainIndex,
                                                        C_OSCNodeDataPoolList & orc_List) const;
   const C_OSCNodeDataPoolListElement * GetUseCaseListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                                   const C_OSCNodeDataPoolList & orc_List)
   const;
   C_OSCNodeDataPoolListElement * GetChanNumListElement(const stw_types::uint32 ou32_DomainIndex,
                                                        C_OSCNodeDataPoolList & orc_List) const;
   const C_OSCNodeDataPoolListElement * GetChanNumListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                                   const C_OSCNodeDataPoolList & orc_List)
   const;
   C_OSCNodeDataPoolListElement * GetSafetyFlagListElement(const stw_types::uint32 ou32_DomainIndex,
                                                           C_OSCNodeDataPoolList & orc_List) const;
   const C_OSCNodeDataPoolListElement * GetSafetyFlagListElementConst(const stw_types::uint32 ou32_DomainIndex,
                                                                      const C_OSCNodeDataPoolList & orc_List)
   const;

   stw_types::uint32 CountRelevantItems(const std::vector<C_OSCHalcConfigChannel> & orc_Channels,
                                        const C_OSCHalcConfigChannel & orc_DomainConfig) const;

   static stw_types::uint32 h_CountElements(const std::vector<C_OSCHalcDefStruct> & orc_Structs);
   static stw_types::uint32 h_CountElements(const C_OSCHalcDefStruct & orc_Struct);
   bool CheckChanPresent(const C_OSCHalcConfigChannel & orc_ChannelConfig) const;
   bool CheckChanRelevant(const C_OSCHalcConfigChannel & orc_ChannelConfig) const;
   bool CheckChanNumVariableNecessary(const C_OSCHalcConfigDomain & orc_ChannelConfig) const;
   bool CheckSafetyFlagVariableNecessary(void) const;
   bool CheckUseCaseVariableNecessary(const C_OSCHalcConfigDomain & orc_Domain) const;

private:
   //lint -e{1725} it is indeed a reference (improves performance)
   const C_OSCHalcConfig & mrc_HalcConfig;
   const C_OSCHalcDefDomain::E_VariableSelector me_Selector;
   const bool mq_IsSafe;

   stw_types::sint32 m_GetListIndex(const stw_types::uint32 ou32_DomainIndex,
                                    const stw_types::uint32 ou32_ParameterStructIndex,
                                    const stw_types::uint32 ou32_ParameterStructElementIndex,
                                    stw_types::uint32 & oru32_ListIndex, const bool oq_GetUseCaseIndex,
                                    const bool oq_GetChanNumIndex, const bool oq_GetSafetyFlagIndex) const;

   static stw_types::sint32 mh_GetSubElementIndex(const stw_types::uint32 ou32_Index,
                                                  const stw_types::uint32 ou32_ElementIndex,
                                                  const std::vector<C_OSCHalcDefStruct> & orc_Values,
                                                  stw_types::uint32 & oru32_ListIndex);
   bool m_CheckIgnoreFlag(void) const;

   //Avoid call
   C_OSCHALCMagicianDatapoolListHandler(const C_OSCHALCMagicianDatapoolListHandler &);
   C_OSCHALCMagicianDatapoolListHandler & operator =(const C_OSCHALCMagicianDatapoolListHandler &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
