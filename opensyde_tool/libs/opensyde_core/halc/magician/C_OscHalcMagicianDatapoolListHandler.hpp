//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class to simplify HALC datapool list access
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCMAGICIANDATAPOOLLISTHANDLER_HPP
#define C_OSCHALCMAGICIANDATAPOOLLISTHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNodeDataPoolList.hpp"
#include "C_OscHalcConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcMagicianDatapoolListHandler
{
public:
   C_OscHalcMagicianDatapoolListHandler(const C_OscHalcConfig & orc_HalcConfig,
                                        const C_OscHalcDefDomain::E_VariableSelector oe_Selector, const bool oq_IsSafe);

   C_OscNodeDataPoolListElement * GetListElement(const uint32_t ou32_DomainIndex,
                                                 const uint32_t ou32_ParameterStructIndex,
                                                 const uint32_t ou32_ParameterStructElementIndex,
                                                 C_OscNodeDataPoolList & orc_List, const uint32_t ou32_CurChannel,
                                                 bool * const opq_IsArray = NULL,
                                                 uint32_t * const opu32_ArrayIndex = NULL) const;
   const C_OscNodeDataPoolListElement * GetListElementConst(const uint32_t ou32_DomainIndex,
                                                            const uint32_t ou32_ParameterStructIndex,
                                                            const uint32_t ou32_ParameterStructElementIndex,
                                                            const C_OscNodeDataPoolList & orc_List,
                                                            const uint32_t ou32_CurChannel,
                                                            bool * const opq_IsArray = NULL,
                                                            uint32_t * const opu32_ArrayIndex = NULL)
   const;
   C_OscNodeDataPoolListElement * GetUseCaseListElement(const uint32_t ou32_DomainIndex,
                                                        C_OscNodeDataPoolList & orc_List) const;
   const C_OscNodeDataPoolListElement * GetUseCaseListElementConst(const uint32_t ou32_DomainIndex,
                                                                   const C_OscNodeDataPoolList & orc_List)
   const;
   C_OscNodeDataPoolListElement * GetChanNumListElement(const uint32_t ou32_DomainIndex,
                                                        C_OscNodeDataPoolList & orc_List) const;
   const C_OscNodeDataPoolListElement * GetChanNumListElementConst(const uint32_t ou32_DomainIndex,
                                                                   const C_OscNodeDataPoolList & orc_List)
   const;
   C_OscNodeDataPoolListElement * GetSafetyFlagListElement(const uint32_t ou32_DomainIndex,
                                                           C_OscNodeDataPoolList & orc_List) const;
   const C_OscNodeDataPoolListElement * GetSafetyFlagListElementConst(const uint32_t ou32_DomainIndex,
                                                                      const C_OscNodeDataPoolList & orc_List)
   const;

   uint32_t CountRelevantItems(const std::vector<C_OscHalcConfigChannel> & orc_Channels,
                               const C_OscHalcConfigChannel & orc_DomainConfig) const;
   std::vector<uint32_t> GetRelevantChannels(const std::vector<C_OscHalcConfigChannel> & orc_Channels,
                                             const C_OscHalcConfigChannel & orc_DomainConfig) const;

   static uint32_t h_CountElements(const std::vector<C_OscHalcDefStruct> & orc_Structs,
                                   const uint32_t ou32_NumChannelsInDatapool);
   static uint32_t h_CountElements(const C_OscHalcDefStruct & orc_Struct, const uint32_t ou32_NumChannelsInDatapool);
   static uint32_t h_CountElements(const C_OscHalcDefElement & orc_Element, const uint32_t ou32_NumChannelsInDatapool);
   bool CheckChanPresent(const C_OscHalcConfigChannel & orc_ChannelConfig) const;
   bool CheckChanRelevant(const C_OscHalcConfigChannel & orc_ChannelConfig) const;
   bool CheckChanNumVariableNecessary(const C_OscHalcConfigDomain & orc_ChannelConfig) const;
   bool CheckSafetyFlagVariableNecessary(void) const;
   bool CheckUseCaseVariableNecessary(const C_OscHalcConfigDomain & orc_Domain) const;

private:
   //lint -e{1725} it is indeed a reference (improves performance)
   const C_OscHalcConfig & mrc_HalcConfig;
   const C_OscHalcDefDomain::E_VariableSelector me_Selector;
   const bool mq_IsSafe;

   int32_t m_GetListIndex(const uint32_t ou32_DomainIndex, const uint32_t ou32_ParameterStructIndex,
                          const uint32_t ou32_ParameterStructElementIndex, uint32_t & oru32_ListIndex,
                          const bool oq_GetUseCaseIndex, const bool oq_GetChanNumIndex,
                          const bool oq_GetSafetyFlagIndex, const uint32_t ou32_CurChannel) const;

   static int32_t mh_GetSubElementIndex(const uint32_t ou32_Index, const uint32_t ou32_ElementIndex,
                                        const uint32_t ou32_CurChannel,
                                        const std::vector<C_OscHalcDefStruct> & orc_Values,
                                        const std::vector<uint32_t> & orc_RelevantChannels, uint32_t & oru32_ListIndex);
   static void mh_GetSubDefElementIndex(const C_OscHalcDefElement & orc_DefElement, const uint32_t ou32_CurChannel,
                                        const std::vector<uint32_t> & orc_RelevantChannels, uint32_t & oru32_ListIndex);
   int32_t m_GetArrayIndexOfChannel(const uint32_t ou32_DomainIndex,
                                    const C_OscNodeDataPoolListElement & orc_ListElement,
                                    const uint32_t ou32_CurChannel, bool & orq_IsArray,
                                    uint32_t & oru32_ArrayIndex) const;
   bool m_CheckIgnoreFlag(void) const;

   //Avoid call
   C_OscHalcMagicianDatapoolListHandler(const C_OscHalcMagicianDatapoolListHandler &);
   C_OscHalcMagicianDatapoolListHandler & operator =(const C_OscHalcMagicianDatapoolListHandler &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
