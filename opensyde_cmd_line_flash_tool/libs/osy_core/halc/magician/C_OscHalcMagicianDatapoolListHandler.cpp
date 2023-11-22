//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class to simplify HALC datapool list access

   Helper class to simplify HALC datapool list access

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscHalcMagicianDatapoolListHandler.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  orc_HalcConfig    Halc config
   \param[in]  oe_Selector       Selector
   \param[in]  oq_IsSafe         Is safe
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcMagicianDatapoolListHandler::C_OscHalcMagicianDatapoolListHandler(const C_OscHalcConfig & orc_HalcConfig,
                                                                           const C_OscHalcDefDomain::E_VariableSelector oe_Selector,
                                                                           const bool oq_IsSafe) :
   mrc_HalcConfig(orc_HalcConfig),
   me_Selector(oe_Selector),
   mq_IsSafe(oq_IsSafe)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list element

   \param[in]      ou32_DomainIndex                   Domain index
   \param[in]      ou32_ParameterStructIndex          Parameter struct index
   \param[in]      ou32_ParameterStructElementIndex   Parameter struct element index
   \param[in,out]  orc_List                           List

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetListElement(const uint32_t ou32_DomainIndex,
                                                                                    const uint32_t ou32_ParameterStructIndex, const uint32_t ou32_ParameterStructElementIndex,
                                                                                    C_OscNodeDataPoolList & orc_List)
const
{
   C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                   ou32_ParameterStructIndex,
                                                   ou32_ParameterStructElementIndex,
                                                   u32_Index, false, false, false);

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

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetListElementConst(
   const uint32_t ou32_DomainIndex, const uint32_t ou32_ParameterStructIndex,
   const uint32_t ou32_ParameterStructElementIndex, const C_OscNodeDataPoolList & orc_List) const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                   ou32_ParameterStructIndex,
                                                   ou32_ParameterStructElementIndex,
                                                   u32_Index, false, false, false);

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

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetUseCaseListElement(
   const uint32_t ou32_DomainIndex, C_OscNodeDataPoolList & orc_List) const
{
   C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                   0UL,
                                                   0UL,
                                                   u32_Index, true, false, false);

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

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetUseCaseListElementConst(
   const uint32_t ou32_DomainIndex, const C_OscNodeDataPoolList & orc_List) const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                   0UL,
                                                   0UL,
                                                   u32_Index, true, false, false);

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

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetChanNumListElement(
   const uint32_t ou32_DomainIndex, C_OscNodeDataPoolList & orc_List) const
{
   C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex, 0UL, 0UL, u32_Index,
                                                   false, true, false);

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

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetChanNumListElementConst(
   const uint32_t ou32_DomainIndex, const C_OscNodeDataPoolList & orc_List) const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                   0UL,
                                                   0UL,
                                                   u32_Index,  false, true, false);

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

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetSafetyFlagListElement(
   const uint32_t ou32_DomainIndex, C_OscNodeDataPoolList & orc_List) const
{
   C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex, 0UL, 0UL, u32_Index,
                                                   false, false, true);

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

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElement * C_OscHalcMagicianDatapoolListHandler::GetSafetyFlagListElementConst(
   const uint32_t ou32_DomainIndex, const C_OscNodeDataPoolList & orc_List) const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   uint32_t u32_Index;
   const int32_t s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                   0UL,
                                                   0UL,
                                                   u32_Index,  false, false, true);

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

   \return
   Number of relevant items
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscHalcMagicianDatapoolListHandler::CountRelevantItems(
   const std::vector<C_OscHalcConfigChannel> & orc_Channels, const C_OscHalcConfigChannel & orc_DomainConfig) const
{
   uint32_t u32_Retval = 0UL;

   if (orc_Channels.size() == 0UL)
   {
      if (this->mrc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING)
      {
         if (orc_DomainConfig.q_SafetyRelevant == this->mq_IsSafe)
         {
            ++u32_Retval;
         }
      }
      else
      {
         ++u32_Retval;
      }
   }
   else
   {
      switch (this->mrc_HalcConfig.e_SafetyMode)
      {
      case C_OscHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING:
         u32_Retval = static_cast<uint32_t>(orc_Channels.size());
         break;
      case C_OscHalcDefBase::eONE_LEVEL_ALL_NON_SAFE:
         if (this->mq_IsSafe == false)
         {
            u32_Retval = static_cast<uint32_t>(orc_Channels.size());
         }
         break;
      case C_OscHalcDefBase::eONE_LEVEL_ALL_SAFE:
         if (this->mq_IsSafe == true)
         {
            u32_Retval = static_cast<uint32_t>(orc_Channels.size());
         }
         break;
      case C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING:
      default:
         for (uint32_t u32_ItChannel = 0UL; u32_ItChannel < orc_Channels.size(); ++u32_ItChannel)
         {
            const C_OscHalcConfigChannel & rc_Channel = orc_Channels[u32_ItChannel];
            if (rc_Channel.q_SafetyRelevant == this->mq_IsSafe)
            {
               ++u32_Retval;
            }
         }
         break;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count elements

   \param[in]  orc_Structs    Structs

   \return
   Number of elements
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscHalcMagicianDatapoolListHandler::h_CountElements(const std::vector<C_OscHalcDefStruct> & orc_Structs)
{
   uint32_t u32_Retval = 0UL;

   for (uint32_t u32_ItStruct = 0UL; u32_ItStruct < orc_Structs.size(); ++u32_ItStruct)
   {
      u32_Retval += C_OscHalcMagicianDatapoolListHandler::h_CountElements(orc_Structs[u32_ItStruct]);
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
uint32_t C_OscHalcMagicianDatapoolListHandler::h_CountElements(const C_OscHalcDefStruct & orc_Struct)
{
   uint32_t u32_Retval = 0UL;

   if (orc_Struct.c_StructElements.size() > 0UL)
   {
      u32_Retval = static_cast<uint32_t>(orc_Struct.c_StructElements.size());
   }
   else
   {
      u32_Retval = 1UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel present

   \param[in]  orc_ChannelConfig    Channel config

   \return
   Flag if channel present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcMagicianDatapoolListHandler::CheckChanPresent(const C_OscHalcConfigChannel & orc_ChannelConfig) const
{
   return this->CheckChanRelevant(orc_ChannelConfig) || this->m_CheckIgnoreFlag();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel relevant

   \param[in]  orc_ChannelConfig    Channel config

   \return
   Flag if channel relevant
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcMagicianDatapoolListHandler::CheckChanRelevant(const C_OscHalcConfigChannel & orc_ChannelConfig) const
{
   return (orc_ChannelConfig.q_SafetyRelevant == this->mq_IsSafe);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel number variable necessary

   \param[in]  orc_ChannelConfig    Channel config

   \return

   \retval   true   Necessary
   \retval   false  Not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcMagicianDatapoolListHandler::CheckChanNumVariableNecessary(const C_OscHalcConfigDomain & orc_ChannelConfig)
const
{
   return (this->me_Selector == C_OscHalcDefDomain::eVA_PARAM) && ((orc_ChannelConfig.c_ChannelConfigs.size() > 0UL) &&
                                                                   (this->mrc_HalcConfig.e_SafetyMode ==
                                                                    C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check safety flag variable necessary

   \return

   \retval   true   Necessary
   \retval   false  Not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcMagicianDatapoolListHandler::CheckSafetyFlagVariableNecessary(void) const
{
   return (this->me_Selector == C_OscHalcDefDomain::eVA_PARAM) &&
          (this->mrc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check use case variable necessary

   \param[in]  orc_Domain  Domain

   \return

   \retval   true   Necessary
   \retval   false  Not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcMagicianDatapoolListHandler::CheckUseCaseVariableNecessary(const C_OscHalcConfigDomain & orc_Domain) const
{
   return (this->me_Selector == C_OscHalcDefDomain::eVA_PARAM) && (orc_Domain.c_ChannelUseCases.size() > 0UL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list index

   \param[in]   ou32_DomainIndex                   Domain index
   \param[in]   ou32_ParameterStructIndex          Parameter struct index
   \param[in]   ou32_ParameterStructElementIndex   Parameter struct element index
   \param[out]  oru32_ListIndex                    List index
   \param[in]   oq_GetUseCaseIndex                 Get use case index
   \param[in]   oq_GetChanNumIndex                 Get channel number index
   \param[in]   oq_GetSafetyFlagIndex              Get safety flag index

   \return
   C_NO_ERR Index found
   C_RANGE  Parameter out of range
   C_CONFIG Config is not in expected format
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcMagicianDatapoolListHandler::m_GetListIndex(const uint32_t ou32_DomainIndex,
                                                             const uint32_t ou32_ParameterStructIndex,
                                                             const uint32_t ou32_ParameterStructElementIndex,
                                                             uint32_t & oru32_ListIndex, const bool oq_GetUseCaseIndex,
                                                             const bool oq_GetChanNumIndex,
                                                             const bool oq_GetSafetyFlagIndex)
const
{
   int32_t s32_Retval = C_NO_ERR;

   oru32_ListIndex = 0UL;

   if (ou32_DomainIndex < this->mrc_HalcConfig.GetDomainSize())
   {
      //Each passed domain
      for (uint32_t u32_ItDomain = 0UL; (u32_ItDomain < ou32_DomainIndex) && (s32_Retval == C_NO_ERR); ++u32_ItDomain)
      {
         const C_OscHalcDefDomain * const pc_DomainDef = this->mrc_HalcConfig.GetDomainDefDataConst(u32_ItDomain);
         const C_OscHalcConfigDomain * const pc_DomainConfig = this->mrc_HalcConfig.GetDomainConfigDataConst(
            u32_ItDomain);
         if ((pc_DomainDef != NULL) && (pc_DomainConfig != NULL))
         {
            const uint32_t u32_CountRelevantItems = this->CountRelevantItems(
               pc_DomainConfig->c_ChannelConfigs, pc_DomainConfig->c_DomainConfig);

            if (u32_CountRelevantItems > 0UL)
            {
               //channels
               if (this->CheckChanNumVariableNecessary(*pc_DomainConfig))
               {
                  oru32_ListIndex += 1UL;
               }

               //safety flag
               if (this->CheckSafetyFlagVariableNecessary())
               {
                  oru32_ListIndex += 1UL;
               }

               //usecase
               if (this->CheckUseCaseVariableNecessary(*pc_DomainConfig))
               {
                  oru32_ListIndex += 1UL;
               }

               //others
               if (pc_DomainConfig->c_ChannelConfigs.size() == 0UL)
               {
                  //domain values
                  switch (this->me_Selector)
                  {
                  case C_OscHalcDefDomain::eVA_PARAM:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_DomainValues.c_Parameters);
                     break;
                  case C_OscHalcDefDomain::eVA_INPUT:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_DomainValues.c_InputValues);
                     break;
                  case C_OscHalcDefDomain::eVA_OUTPUT:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_DomainValues.c_OutputValues);
                     break;
                  case C_OscHalcDefDomain::eVA_STATUS:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_DomainValues.c_StatusValues);
                     break;
                  default:
                     break;
                  }
               }
               else
               {
                  //channel values
                  switch (this->me_Selector)
                  {
                  case C_OscHalcDefDomain::eVA_PARAM:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_ChannelValues.c_Parameters);
                     break;
                  case C_OscHalcDefDomain::eVA_INPUT:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_ChannelValues.c_InputValues);
                     break;
                  case C_OscHalcDefDomain::eVA_OUTPUT:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_ChannelValues.c_OutputValues);
                     break;
                  case C_OscHalcDefDomain::eVA_STATUS:
                     oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
                        pc_DomainDef->c_ChannelValues.c_StatusValues);
                     break;
                  default:
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

      if (((oq_GetUseCaseIndex == false) && (oq_GetChanNumIndex == false)) && (oq_GetSafetyFlagIndex == false))
      {
         const C_OscHalcDefDomain * const pc_CurrentDomainDef =
            this->mrc_HalcConfig.GetDomainDefDataConst(ou32_DomainIndex);
         const C_OscHalcConfigDomain * const pc_CurrentDomainConfig = this->mrc_HalcConfig.GetDomainConfigDataConst(
            ou32_DomainIndex);
         //Current domain
         if ((pc_CurrentDomainDef != NULL) && (pc_CurrentDomainConfig != NULL))
         {
            const uint32_t u32_CountRelevantItems = C_OscHalcMagicianDatapoolListHandler::CountRelevantItems(
               pc_CurrentDomainConfig->c_ChannelConfigs, pc_CurrentDomainConfig->c_DomainConfig);
            if (u32_CountRelevantItems > 0UL)
            {
               //channels
               if (C_OscHalcMagicianDatapoolListHandler::CheckChanNumVariableNecessary(*pc_CurrentDomainConfig))
               {
                  oru32_ListIndex += 1UL;
               }

               //safety flag
               if (this->CheckSafetyFlagVariableNecessary())
               {
                  oru32_ListIndex += 1UL;
               }

               //usecase
               if (this->CheckUseCaseVariableNecessary(*pc_CurrentDomainConfig))
               {
                  oru32_ListIndex += 1UL;
               }

               //others
               switch (this->me_Selector)
               {
               case C_OscHalcDefDomain::eVA_PARAM:
                  if (C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                         ou32_ParameterStructIndex,
                         ou32_ParameterStructElementIndex,
                         pc_CurrentDomainDef->c_DomainValues.c_Parameters,
                         oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                        ou32_ParameterStructIndex,
                        ou32_ParameterStructElementIndex,
                        pc_CurrentDomainDef->c_ChannelValues.c_Parameters,
                        oru32_ListIndex);
                  }
                  break;
               case C_OscHalcDefDomain::eVA_INPUT:
                  if (C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                         ou32_ParameterStructIndex,
                         ou32_ParameterStructElementIndex,
                         pc_CurrentDomainDef->c_DomainValues.c_InputValues,
                         oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                        ou32_ParameterStructIndex,
                        ou32_ParameterStructElementIndex,
                        pc_CurrentDomainDef->c_ChannelValues.c_InputValues,
                        oru32_ListIndex);
                  }
                  break;
               case C_OscHalcDefDomain::eVA_OUTPUT:
                  if (C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                         ou32_ParameterStructIndex,
                         ou32_ParameterStructElementIndex,
                         pc_CurrentDomainDef->c_DomainValues.
                         c_OutputValues,
                         oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                        ou32_ParameterStructIndex,
                        ou32_ParameterStructElementIndex,
                        pc_CurrentDomainDef->c_ChannelValues.c_OutputValues,
                        oru32_ListIndex);
                  }
                  break;
               case C_OscHalcDefDomain::eVA_STATUS:
                  if (C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                         ou32_ParameterStructIndex,
                         ou32_ParameterStructElementIndex,
                         pc_CurrentDomainDef->c_DomainValues.c_StatusValues,
                         oru32_ListIndex) == C_RANGE)
                  {
                     s32_Retval = C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(
                        ou32_ParameterStructIndex,
                        ou32_ParameterStructElementIndex,
                        pc_CurrentDomainDef->c_ChannelValues.c_StatusValues,
                        oru32_ListIndex);
                  }
                  break;
               default:
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
      else if (((oq_GetUseCaseIndex == true) && (oq_GetChanNumIndex == false)) && (oq_GetSafetyFlagIndex == false))
      {
         const C_OscHalcConfigDomain * const pc_DomainConfig = this->mrc_HalcConfig.GetDomainConfigDataConst(
            ou32_DomainIndex);
         if ((pc_DomainConfig != NULL) &&
             (C_OscHalcMagicianDatapoolListHandler::CheckChanNumVariableNecessary(*pc_DomainConfig)))
         {
            //channels
            oru32_ListIndex += 1UL;
         }
         if (this->CheckSafetyFlagVariableNecessary())
         {
            //safety
            oru32_ListIndex += 1UL;
         }
      }
      else if (((oq_GetUseCaseIndex == false) && (oq_GetChanNumIndex == false)) && (oq_GetSafetyFlagIndex == true))
      {
         const C_OscHalcConfigDomain * const pc_DomainConfig = this->mrc_HalcConfig.GetDomainConfigDataConst(
            ou32_DomainIndex);
         if ((pc_DomainConfig != NULL) &&
             (C_OscHalcMagicianDatapoolListHandler::CheckChanNumVariableNecessary(*pc_DomainConfig)))
         {
            //channels
            oru32_ListIndex += 1UL;
         }
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
int32_t C_OscHalcMagicianDatapoolListHandler::mh_GetSubElementIndex(const uint32_t ou32_Index,
                                                                    const uint32_t ou32_ElementIndex,
                                                                    const std::vector<C_OscHalcDefStruct> & orc_Values,
                                                                    uint32_t & oru32_ListIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < orc_Values.size())
   {
      //Passed structs
      for (uint32_t u32_ItStruct = 0UL; u32_ItStruct < ou32_Index; ++u32_ItStruct)
      {
         oru32_ListIndex += C_OscHalcMagicianDatapoolListHandler::h_CountElements(
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Ignore flag scenario

   \return
   Is ignore flag scenario
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcMagicianDatapoolListHandler::m_CheckIgnoreFlag(void) const
{
   return ((this->mrc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITHOUT_DROPPING) ||
           (this->mrc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_NON_SAFE)) ||
          (this->mrc_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_SAFE);
}
