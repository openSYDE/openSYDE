//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for common node CAN related functionalities

   Utility class for common node CAN related functionalities

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdNodeCanUtil.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeCanUtil::C_PuiSdNodeCanUtil()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set can signal position part

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
   \param[in]  orc_OscSignal        Signal data (osc)
   \param[in]  orc_UiSignal         Signal data (ui)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdNodeCanUtil::h_SetCanSignalPosition(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                   const uint32_t & oru32_SignalIndex,
                                                   const C_OscCanSignal & orc_OscSignal,
                                                   const C_PuiSdNodeCanSignal & orc_UiSignal)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscNodeDataPoolListElementId c_Id;

   if (C_PuiSdUtil::h_ConvertFromSignalIndex(orc_MessageId, oru32_SignalIndex, c_Id) == C_NO_ERR)
   {
      const C_OscNodeDataPoolListElement * const pc_OscElement =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id);
      const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
         C_PuiSdHandler::h_GetInstance()->GetUiDataPoolListElement(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                   c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
      if ((pc_OscElement != NULL) && (pc_UiElement != NULL))
      {
         if (pc_UiElement->q_AutoMinMaxActive)
         {
            C_OscNodeDataPoolListElement c_NewElement = *pc_OscElement;
            //Type
            const E_SignalType e_SignalType = C_PuiSdNodeCanUtil::h_GetSignalType(c_NewElement.GetType());
            const C_OscNodeDataPoolContent::E_Type e_NewType = C_PuiSdNodeCanUtil::h_GetRequiredType(
               orc_OscSignal.u16_ComBitLength, e_SignalType);
            c_NewElement.c_MinValue.SetType(e_NewType);
            c_NewElement.c_MaxValue.SetType(e_NewType);
            c_NewElement.c_DataSetValues[0].SetType(e_NewType);
            //Value
            C_SdNdeDpContentUtil::h_InitMinForSignal(c_NewElement.c_MinValue, orc_OscSignal.u16_ComBitLength);
            C_SdNdeDpContentUtil::h_InitMaxForSignal(c_NewElement.c_MaxValue, orc_OscSignal.u16_ComBitLength);
            C_PuiSdNodeCanUtil::h_AdaptValueToSignalLength(orc_OscSignal.u16_ComBitLength,
                                                           c_NewElement.c_DataSetValues[0]);
            s32_Retval  = C_PuiSdHandler::h_GetInstance()->SetCanSignal(orc_MessageId, oru32_SignalIndex,
                                                                        orc_OscSignal, c_NewElement, *pc_UiElement,
                                                                        orc_UiSignal);
         }
         else
         {
            s32_Retval  = C_PuiSdHandler::h_GetInstance()->SetCanSignal(orc_MessageId, oru32_SignalIndex,
                                                                        orc_OscSignal, *pc_OscElement, *pc_UiElement,
                                                                        orc_UiSignal);
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt value to signal length

   \param[in]      ou16_BitLength   Current bit length
   \param[in,out]  orc_Content      Content to restrict
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanUtil::h_AdaptValueToSignalLength(const uint16_t ou16_BitLength,
                                                    C_OscNodeDataPoolContent & orc_Content)
{
   C_OscNodeDataPoolContent c_Min;
   C_OscNodeDataPoolContent c_Max;

   c_Min.SetArray(false);
   c_Min.SetType(orc_Content.GetType());
   C_SdNdeDpContentUtil::h_InitMinForSignal(c_Min, ou16_BitLength);

   c_Max.SetArray(false);
   c_Max.SetType(orc_Content.GetType());
   C_SdNdeDpContentUtil::h_InitMaxForSignal(c_Max, ou16_BitLength);

   if (orc_Content < c_Min)
   {
      orc_Content = c_Min;
   }
   if (orc_Content > c_Max)
   {
      orc_Content = c_Max;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get required type

   \param[in]  ou16_BitLength    Bit length
   \param[in]  oe_SignalType     Signal type

   \return
   Required type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_PuiSdNodeCanUtil::h_GetRequiredType(const uint16_t ou16_BitLength,
                                                                       const E_SignalType oe_SignalType)
{
   C_OscNodeDataPoolContent::E_Type e_Retval = C_OscNodeDataPoolContent::eUINT8;

   switch (oe_SignalType)
   {
   case eST_UNSIGNED:
      if (ou16_BitLength <= 8U)
      {
         e_Retval = C_OscNodeDataPoolContent::eUINT8;
      }
      else if ((ou16_BitLength <= 16U) && (ou16_BitLength > 8U))
      {
         e_Retval = C_OscNodeDataPoolContent::eUINT16;
      }
      else if ((ou16_BitLength <= 32U) && (ou16_BitLength > 16U))
      {
         e_Retval = C_OscNodeDataPoolContent::eUINT32;
      }
      else
      {
         e_Retval = C_OscNodeDataPoolContent::eUINT64;
      }
      break;
   case eST_SIGNED:
      if (ou16_BitLength <= 8U)
      {
         e_Retval = C_OscNodeDataPoolContent::eSINT8;
      }
      else if ((ou16_BitLength <= 16U) && (ou16_BitLength > 8U))
      {
         e_Retval = C_OscNodeDataPoolContent::eSINT16;
      }
      else if ((ou16_BitLength <= 32U) && (ou16_BitLength > 16U))
      {
         e_Retval = C_OscNodeDataPoolContent::eSINT32;
      }
      else
      {
         e_Retval = C_OscNodeDataPoolContent::eSINT64;
      }
      break;
   case eST_FLOAT32:
      e_Retval = C_OscNodeDataPoolContent::eFLOAT32;
      break;
   case eST_FLOAT64:
      e_Retval = C_OscNodeDataPoolContent::eFLOAT64;
      break;
   default:
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get signal type

   \param[in]  oe_DpType   Datapool type

   \return
   Signal type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeCanUtil::E_SignalType C_PuiSdNodeCanUtil::h_GetSignalType(const C_OscNodeDataPoolContent::E_Type oe_DpType)
{
   C_PuiSdNodeCanUtil::E_SignalType e_Retval = C_PuiSdNodeCanUtil::eST_SIGNED;
   switch (oe_DpType)
   {
   case C_OscNodeDataPoolContent::eSINT8:
   case C_OscNodeDataPoolContent::eSINT16:
   case C_OscNodeDataPoolContent::eSINT32:
   case C_OscNodeDataPoolContent::eSINT64:
      e_Retval = C_PuiSdNodeCanUtil::eST_SIGNED;
      break;
   case C_OscNodeDataPoolContent::eUINT8:
   case C_OscNodeDataPoolContent::eUINT16:
   case C_OscNodeDataPoolContent::eUINT32:
   case C_OscNodeDataPoolContent::eUINT64:
      e_Retval = C_PuiSdNodeCanUtil::eST_UNSIGNED;
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
      e_Retval = C_PuiSdNodeCanUtil::eST_FLOAT32;
      break;
   case C_OscNodeDataPoolContent::eFLOAT64:
      e_Retval = C_PuiSdNodeCanUtil::eST_FLOAT64;
      break;
   default:
      break;
   }

   return e_Retval;
}
