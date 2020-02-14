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

   \param[in]  orc_HalcDef    Halc def
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHALCMagicianDatapoolListHandler::C_OSCHALCMagicianDatapoolListHandler(const C_OSCHalcDefBase & orc_HalcDef) :
   mrc_HalcDef(orc_HalcDef)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list element

   \param[in]      ou32_DomainIndex                   Domain index
   \param[in]      ou32_ParameterStructIndex          Parameter struct index
   \param[in]      ou32_ParameterStructElementIndex   Parameter struct element index
   \param[in,out]  orc_List                           List
   \param[in]      oe_Selector                        Selector

   \return
   List element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetListElement(const uint32 ou32_DomainIndex,
                                                                                    const uint32 ou32_ParameterStructIndex, const uint32 ou32_ParameterStructElementIndex, C_OSCNodeDataPoolList & orc_List,
                                                                                    const C_OSCHalcDefDomain::E_VariableSelector oe_Selector)
const
{
   C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  ou32_ParameterStructIndex,
                                                  ou32_ParameterStructElementIndex,
                                                  u32_Index, oe_Selector, false);

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

   \return
   List element const
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetListElementConst(
   const uint32 ou32_DomainIndex, const uint32 ou32_ParameterStructIndex, const uint32 ou32_ParameterStructElementIndex,
   const C_OSCNodeDataPoolList & orc_List, const C_OSCHalcDefDomain::E_VariableSelector oe_Selector) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  ou32_ParameterStructIndex,
                                                  ou32_ParameterStructElementIndex,
                                                  u32_Index, oe_Selector, false);

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
C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetUseCaseListElement(
   const uint32 ou32_DomainIndex, C_OSCNodeDataPoolList & orc_List) const
{
   C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  0UL,
                                                  0UL,
                                                  u32_Index, C_OSCHalcDefDomain::eVA_PARAM, true);

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
const C_OSCNodeDataPoolListElement * C_OSCHALCMagicianDatapoolListHandler::GetUseCaseListElementConst(
   const uint32 ou32_DomainIndex, const C_OSCNodeDataPoolList & orc_List) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   uint32 u32_Index;
   const sint32 s32_Result = this->m_GetListIndex(ou32_DomainIndex,
                                                  0UL,
                                                  0UL,
                                                  u32_Index, C_OSCHalcDefDomain::eVA_PARAM, true);

   if ((s32_Result == C_NO_ERR) && (u32_Index < orc_List.c_Elements.size()))
   {
      pc_Retval = &orc_List.c_Elements[u32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list index

   \param[in]   ou32_DomainIndex                   Domain index
   \param[in]   ou32_ParameterStructIndex          Parameter struct index
   \param[in]   ou32_ParameterStructElementIndex   Parameter struct element index
   \param[out]  oru32_ListIndex                    List index
   \param[in]   oe_Selector                        Selector
   \param[in]   oq_GetUseCaseIndex                 Get use case index

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
                                                            const bool oq_GetUseCaseIndex)
const
{
   sint32 s32_Retval = C_NO_ERR;

   oru32_ListIndex = 0UL;

   if (ou32_DomainIndex < this->mrc_HalcDef.GetDomainSize())
   {
      //Each passed domain
      for (uint32 u32_ItDomain = 0UL; (u32_ItDomain < ou32_DomainIndex) && (s32_Retval == C_NO_ERR); ++u32_ItDomain)
      {
         const C_OSCHalcDefDomain * const pc_Domain = this->mrc_HalcDef.GetDomainDefDataConst(u32_ItDomain);
         if (pc_Domain != NULL)
         {
            //usecase
            if (pc_Domain->c_ChannelUseCases.size() > 0UL)
            {
               oru32_ListIndex += 1UL;
            }
            //others
            switch (oe_Selector)
            {
            case C_OSCHalcDefDomain::eVA_PARAM:
               oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(pc_Domain->c_Parameters);
               break;
            case C_OSCHalcDefDomain::eVA_INPUT:
               oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(pc_Domain->c_InputValues);
               break;
            case C_OSCHalcDefDomain::eVA_OUTPUT:
               oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(pc_Domain->c_OutputValues);
               break;
            case C_OSCHalcDefDomain::eVA_STATUS:
               oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(pc_Domain->c_StatusValues);
               break;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }

      if (oq_GetUseCaseIndex == false)
      {
         const C_OSCHalcDefDomain * const pc_CurrentDomain = this->mrc_HalcDef.GetDomainDefDataConst(ou32_DomainIndex);
         //Current domain
         if (pc_CurrentDomain != NULL)
         {
            //usecase
            if (pc_CurrentDomain->c_ChannelUseCases.size() > 0UL)
            {
               oru32_ListIndex += 1UL;
            }
            //others
            switch (oe_Selector)
            {
            case C_OSCHalcDefDomain::eVA_PARAM:
               if (ou32_ParameterStructIndex < pc_CurrentDomain->c_Parameters.size())
               {
                  //Passed structs
                  for (uint32 u32_ItStruct = 0UL; u32_ItStruct < ou32_ParameterStructIndex; ++u32_ItStruct)
                  {
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_CurrentDomain->c_Parameters[u32_ItStruct]);
                  }
                  //Current struct
                  oru32_ListIndex += ou32_ParameterStructElementIndex;
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
               break;
            case C_OSCHalcDefDomain::eVA_INPUT:
               if (ou32_ParameterStructIndex < pc_CurrentDomain->c_InputValues.size())
               {
                  //Passed structs
                  for (uint32 u32_ItStruct = 0UL; u32_ItStruct < ou32_ParameterStructIndex; ++u32_ItStruct)
                  {
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_CurrentDomain->c_InputValues[u32_ItStruct]);
                  }
                  //Current struct
                  oru32_ListIndex += ou32_ParameterStructElementIndex;
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
               break;
            case C_OSCHalcDefDomain::eVA_OUTPUT:
               if (ou32_ParameterStructIndex < pc_CurrentDomain->c_OutputValues.size())
               {
                  //Passed structs
                  for (uint32 u32_ItStruct = 0UL; u32_ItStruct < ou32_ParameterStructIndex; ++u32_ItStruct)
                  {
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_CurrentDomain->c_OutputValues[u32_ItStruct]);
                  }
                  //Current struct
                  oru32_ListIndex += ou32_ParameterStructElementIndex;
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
               break;
            case C_OSCHalcDefDomain::eVA_STATUS:
               if (ou32_ParameterStructIndex < pc_CurrentDomain->c_StatusValues.size())
               {
                  //Passed structs
                  for (uint32 u32_ItStruct = 0UL; u32_ItStruct < ou32_ParameterStructIndex; ++u32_ItStruct)
                  {
                     oru32_ListIndex += C_OSCHALCMagicianDatapoolListHandler::mh_CountElements(
                        pc_CurrentDomain->c_StatusValues[u32_ItStruct]);
                  }
                  //Current struct
                  oru32_ListIndex += ou32_ParameterStructElementIndex;
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
               break;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
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
