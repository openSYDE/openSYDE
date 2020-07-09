//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class to simplify HALC datapool list access

   Helper class to simplify HALC datapool list access

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCHALCMagicianDatapoolListHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor/destructor

   \param[in]  orc_HalcConfig    Halc config
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHALCMagicianDatapoolListHandler::C_OSCHALCMagicianDatapoolListHandler(const C_OSCHalcConfig & orc_HalcConfig) :
   mrc_HalcConfig(orc_HalcConfig)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list element

   \param[in]      ou32_DomainIndex                   Domain index
   \param[in]      ou32_ParameterStructIndex          Parameter struct index
   \param[in]      ou32_ParameterStructElementIndex   Parameter struct element index
   \param[in,out]  orc_List                           List
   \param[in]      oe_Selector                        Selector
   \param[in]      oq_IsSafe                          Is safe

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetListElement(const uint32 ou32_DomainIndex,
                                                                                    const uint32 ou32_ParameterStructIndex, const uint32 ou32_ParameterStructElementIndex, C_OSCNodeDataPoolList & orc_List, const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                                                                    const bool oq_IsSafe)
const
{
   C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  ou32_ParameterStructIndex,
                                                  ou32_ParameterStructElementIndex,
                                                  u32_Index, oe_Selector, false, false, oq_IsSafe);

   if ((s32_Result == C_NO_ERR) && (u32_Index < orc_List.c_Elements.size()))
   {
      pc_Retval = &orc_List.c_Elements[u32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list element const

   \param[in]      ou32_DomainIndex                   Domain index
   \param[in]      ou32_ParameterStructIndex          Parameter struct index
   \param[in]      ou32_ParameterStructElementIndex   Parameter struct element index
   \param[in,out]  orc_List                           List
   \param[in]      oe_Selector                        Selector
   \param[in]      oq_IsSafe                          Is safe

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetListElementConst(
   const uint32 ou32_DomainIndex, const uint32 ou32_ParameterStructIndex, const uint32 ou32_ParameterStructElementIndex,
   const C_OSCNodeDataPoolList & orc_List, const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
   const bool oq_IsSafe) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  ou32_ParameterStructIndex,
                                                  ou32_ParameterStructElementIndex,
                                                  u32_Index, oe_Selector, false, false, oq_IsSafe);

   if ((s32_Result == C_NO_ERR) && (u32_Index < orc_List.c_Elements.size()))
   {
      pc_Retval = &orc_List.c_Elements[u32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get use case list element

   \param[in]      ou32_DomainIndex    Domain index
   \param[in,out]  orc_List            List
   \param[in]      oq_IsSafe           Is safe

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetUseCaseListElement(
   const uint32 ou32_DomainIndex, C_OSCNodeDataPoolList & orc_List, const bool oq_IsSafe) const
{
   C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  0UL,
                                                  0UL,
                                                  u32_Index, C_OSCHalcDefDomain::eVA_PARAM, true, false, oq_IsSafe);

   if ((s32_Result == C_NO_ERR) && (u32_Index < orc_List.c_Elements.size()))
   {
      pc_Retval = &orc_List.c_Elements[u32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get use case list element const

   \param[in]  ou32_DomainIndex  Domain index
   \param[in]  orc_List          List
   \param[in]  oq_IsSafe         Is safe

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetUseCaseListElementConst(
   const uint32 ou32_DomainIndex, const C_OSCNodeDataPoolList & orc_List, const bool oq_IsSafe) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  0UL,
                                                  0UL,
                                                  u32_Index, C_OSCHalcDefDomain::eVA_PARAM, true, false, oq_IsSafe);

   if ((s32_Result == C_NO_ERR) && (u32_Index < orc_List.c_Elements.size()))
   {
      pc_Retval = &orc_List.c_Elements[u32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get channel number list element

   \param[in]      ou32_DomainIndex    Domain index
   \param[in,out]  orc_List            List
   \param[in]      oq_IsSafe           Is safe

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetChanNumListElement(
   const uint32 ou32_DomainIndex, C_OSCNodeDataPoolList & orc_List, const bool oq_IsSafe) const
{
   C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  0UL,
                                                  0UL,
                                                  u32_Index, C_OSCHalcDefDomain::eVA_PARAM, false, true, oq_IsSafe);

   if ((s32_Result == C_NO_ERR) && (u32_Index < orc_List.c_Elements.size()))
   {
      pc_Retval = &orc_List.c_Elements[u32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get channel number list element const

   \param[in]  ou32_DomainIndex  Domain index
   \param[in]  orc_List          List
   \param[in]  oq_IsSafe         Is safe

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetChanNumListElementConst(
   const uint32 ou32_DomainIndex, const C_OSCNodeDataPoolList & orc_List, const bool oq_IsSafe) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  0UL,
                                                  0UL,
                                                  u32_Index, C_OSCHalcDefDomain::eVA_PARAM, false, true, oq_IsSafe);

   if ((s32_Result == C_NO_ERR) && (u32_Index < orc_List.c_Elements.size()))
   {
      pc_Retval = &orc_List.c_Elements[u32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count relevant items

   \param[in]  orc_Channels      Channels
   \param[in]  orc_DomainConfig  Domain config
   \param[in]  oq_IsSafe         Is safe

   \return
   Number of relevant items
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCHALCMagicianDatapoolListHandler::h_CountRelevantItems(
   const std::vector<C_OSCHalcConfigChannel> & orc_Channels, const C_OSCHalcConfigChannel & orc_DomainConfig,
   const bool oq_IsSafe)
{
   uint32 u32_Retval = 0UL;

   for (uint32 u32_ItChannel = 0UL; u32_ItChannel < orc_Channels.size(); ++u32_ItChannel)
   {
      const C_OSCHalcConfigChannel & rc_Channel = orc_Channels[u32_ItChannel];
      if (rc_Channel.q_SafetyRelevant == oq_IsSafe)
      {
         ++u32_Retval;
      }
   }
   if (orc_Channels.size() == 0UL)
   {
      if (orc_DomainConfig.q_SafetyRelevant == oq_IsSafe)
      {
         ++u32_Retval;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list index

   \param[in]   ou32_DomainIndex                   Domain index
   \param[in]   ou32_ParameterStructIndex          Parameter struct index
   \param[in]   ou32_ParameterStructElementIndex   Parameter struct element index
   \param[out]  oru32_ListIndex                    List index
   \param[in]   oe_Selector                        Selector
   \param[in]   oq_GetUseCaseIndex                 Get use case index
   \param[in]   oq_GetChanNumIndex                 Get channel number index
   \param[in]   oq_IsSafe                          Is safe

   \return
   C_NO_ERR Index found
   C_RANGE  Parameter out of range
   C_CONFIG Config is not in expected format
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianDatapoolListHandler::m_GetListIndex(const uint32 ou32_DomainIndex,
                                                            const uint32 ou32_ParameterStructIndex,
                                                            const uint32 ou32_ParameterStructElementIndex,
                                                            uint32 & oru32_ListIndex,
                                                            const C_OSCHalcDefDomain::E_VariableSelector oe_Selector,
                                                            const bool oq_GetUseCaseIndex,
                                                            const bool oq_GetChanNumIndex, const bool oq_IsSafe)
const
{
   sint32 s32_Retval = C_NO_ERR;

   oru32_ListIndex = 0UL;

   if (ou32_DomainIndex < this->mrc_HalcConfig.GetDomainSize())
   {
      //Each passed domain
      for (uint32 u32_ItDomain = 0UL; (u32_ItDomain < ou32_DomainIndex) && (s32_Retval == C_NO_ERR); ++u32_ItDomain)
      {
         const C_OSCHalcDefDomain * const pc_DomainDef = this->mrc_HalcConfig.GetDomainDefDataConst(u32_ItDomain);
         const C_OSCHalcConfigDomain * const pc_DomainConfig = this->mrc_HalcConfig.GetDomainConfigDataConst(
            u32_ItDomain);
         if ((pc_DomainDef != NULL) && (pc_DomainConfig != NULL))
         {
            const uint32 u32_CountRelevantItems = C_OSCHALCMagicianDatapoolListHandler::h_CountRelevantItems(
               pc_DomainConfig->c_ChannelConfigs, pc_DomainConfig->c_DomainConfig, oq_IsSafe);

            if (u32_CountRelevantItems > 0UL)
            {
               //channels
               if (pc_DomainConfig->c_ChannelConfigs.size() > 0UL)
               {
                  oru32_ListIndex += 1UL;
               }

               //usecase
               if (pc_DomainConfig->c_ChannelUseCases.size() > 0UL)
               {
                  oru32_ListIndex += 1UL;
               }

               //others
               if (pc_DomainConfig->c_ChannelConfigs.size() == 0UL)
               {
                  //domain values
                  switch (oe_Selector)
                  {
                  case C_OSCHalcDefDomain::eVA_PARAM:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_DomainValues.c_Parameters);
                     break;
                  case C_OSCHalcDefDomain::eVA_INPUT:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_DomainValues.c_InputValues);
                     break;
                  case C_OSCHalcDefDomain::eVA_OUTPUT:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_DomainValues.c_OutputValues);
                     break;
                  case C_OSCHalcDefDomain::eVA_STATUS:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_DomainValues.c_StatusValues);
                     break;
                  }
               }
               else
               {
                  //channel values
                  switch (oe_Selector)
                  {
                  case C_OSCHalcDefDomain::eVA_PARAM:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_ChannelValues.c_Parameters);
                     break;
                  case C_OSCHalcDefDomain::eVA_INPUT:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_ChannelValues.c_InputValues);
                     break;
                  case C_OSCHalcDefDomain::eVA_OUTPUT:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_ChannelValues.c_OutputValues);
                     break;
                  case C_OSCHalcDefDomain::eVA_STATUS:
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_DomainDef->c_ChannelValues.c_StatusValues);
                     break;
                  }
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }

      if ((oq_GetUseCaseIndex == false) && (oq_GetChanNumIndex == false))
      {
         const C_OSCHalcDefDomain * const pc_CurrentDomain =
            this->mrc_HalcConfig.GetDomainDefDataConst(ou32_DomainIndex);
         const C_OSCHalcConfigDomain * const pc_CurrentDomainConfig = this->mrc_HalcConfig.GetDomainConfigDataConst(
            ou32_DomainIndex);
         //Current domain
         if ((pc_CurrentDomain != NULL) && (pc_CurrentDomainConfig != NULL))
         {
            const uint32 u32_CountRelevantItems = C_OSCHALCMagicianDatapoolListHandler::h_CountRelevantItems(
               pc_CurrentDomainConfig->c_ChannelConfigs, pc_CurrentDomainConfig->c_DomainConfig, oq_IsSafe);
            if (u32_CountRelevantItems > 0UL)
            {
               //channels
               if (pc_CurrentDomainConfig->c_ChannelConfigs.size() > 0UL)
               {
                  oru32_ListIndex += 1UL;
               }

               //usecase
               if (pc_CurrentDomain->c_ChannelUseCases.size() > 0UL)
               {
                  oru32_ListIndex += 1UL;
               }

               //others
               switch (oe_Selector)
               {
               case C_OSCHalcDefDomain::eVA_PARAM:
                  if (C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                  ou32_ParameterStructElementIndex,
                                                                                  pc_CurrentDomain->c_DomainValues.
                                                                                  c_Parameters,
                                                                                  oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                              ou32_ParameterStructElementIndex,
                                                                                              pc_CurrentDomain->c_ChannelValues.c_Parameters,
                                                                                              oru32_ListIndex);
                  }
                  break;
               case C_OSCHalcDefDomain::eVA_INPUT:
                  if (C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                  ou32_ParameterStructElementIndex,
                                                                                  pc_CurrentDomain->c_DomainValues.
                                                                                  c_InputValues,
                                                                                  oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                              ou32_ParameterStructElementIndex,
                                                                                              pc_CurrentDomain->c_ChannelValues.c_InputValues,
                                                                                              oru32_ListIndex);
                  }
                  break;
               case C_OSCHalcDefDomain::eVA_OUTPUT:
                  if (C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                  ou32_ParameterStructElementIndex,
                                                                                  pc_CurrentDomain->c_DomainValues.
                                                                                  c_OutputValues,
                                                                                  oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                              ou32_ParameterStructElementIndex,
                                                                                              pc_CurrentDomain->c_ChannelValues.c_OutputValues,
                                                                                              oru32_ListIndex);
                  }
                  break;
               case C_OSCHalcDefDomain::eVA_STATUS:
                  if (C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                  ou32_ParameterStructElementIndex,
                                                                                  pc_CurrentDomain->c_DomainValues.
                                                                                  c_StatusValues,
                                                                                  oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(ou32_ParameterStructIndex,
                                                                                              ou32_ParameterStructElementIndex,
                                                                                              pc_CurrentDomain->c_ChannelValues.c_StatusValues,
                                                                                              oru32_ListIndex);
                  }
                  break;
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else if ((oq_GetUseCaseIndex == true) && (oq_GetChanNumIndex == false))
      {
         //channels
         oru32_ListIndex += 1UL;
      }
      else
      {
         //use current index
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count elements

   \param[in]  orc_Structs    Structs

   \return
   Number of elements
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(const std::vector<C_OSCHalcDefStruct> & orc_Structs)
{
   uint32 u32_Retval = 0UL;

   for (uint32 u32_ItStruct = 0UL; u32_ItStruct < orc_Structs.size(); ++u32_ItStruct)
   {
      u32_Retval += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(orc_Structs[u32_ItStruct]);
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count elements

   \param[in]  orc_Struct  Struct

   \return
   Number of elements
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(const C_OSCHalcDefStruct & orc_Struct)
{
   uint32 u32_Retval = 0UL;

   if (orc_Struct.c_StructElements.size() > 0UL)
   {
      u32_Retval = orc_Struct.c_StructElements.size();
   }
   else
   {
      u32_Retval = 1UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count sub elements

   \param[in]   ou32_Index          Index
   \param[in]   ou32_ElementIndex   Element index
   \param[in]   orc_Values          Values
   \param[out]  oru32_ListIndex     List index

   \return
   C_NO_ERR Index found
   C_RANGE  Parameter out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHALCMagicianDatapoolListHandler::mh_GetSubElementIndex(const uint32 ou32_Index,
                                                                   const uint32 ou32_ElementIndex,
                                                                   const std::vector<C_OSCHalcDefStruct> & orc_Values,
                                                                   uint32 & oru32_ListIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < orc_Values.size())
   {
      //Passed structs
      for (uint32 u32_ItStruct = 0UL; u32_ItStruct < ou32_Index; ++u32_ItStruct)
      {
         oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
            orc_Values[u32_ItStruct]);
      }
      //Current struct
      oru32_ListIndex += ou32_ElementIndex;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
