//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to centralize tooltip handling (implementation)

   Utility class to centralize tooltip handling

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_SdTooltipUtil.h"
#include "C_SdNdeDpContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for tooltip content for message.

   \param[in] orc_Message Message for which the tooltip is requested

   \return
   Content as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdTooltipUtil::h_GetToolTipContentMessage(const stw_opensyde_core::C_OSCCanMessage & orc_Message)
{
   QString c_ToolTipContent;
   QString c_Tmp;

   // Comment
   if (orc_Message.c_Comment.IsEmpty() == false)
   {
      c_ToolTipContent = orc_Message.c_Comment.c_str();
      c_ToolTipContent.append("\n\n");
   }

   //CAN settings
   c_ToolTipContent += C_GtGetText::h_GetText("Message Properties:\n");
   if (orc_Message.q_IsExtended == true)
   {
      c_Tmp = C_GtGetText::h_GetText("Extended (29 bit)");
   }
   else
   {
      c_Tmp = C_GtGetText::h_GetText("Standard (11 bit)");
   }
   c_ToolTipContent += static_cast<QString>(static_cast<QString>("   ") + C_GtGetText::h_GetText("Type: %1\n")).arg(
      c_Tmp);
   c_ToolTipContent +=
      static_cast<QString>(static_cast<QString>("   ") +
                           C_GtGetText::h_GetText("CAN ID: 0x%1\n")).arg(static_cast<QString>("%1").arg(orc_Message.
                                                                                                        u32_CanId, 0,
                                                                                                        16).toUpper());

   c_ToolTipContent +=
      static_cast<QString>(static_cast<QString>("   ") + C_GtGetText::h_GetText("DLC: %1 Bytes\n")).arg(
         orc_Message.u16_Dlc);
   switch (orc_Message.e_TxMethod)
   {
   case C_OSCCanMessage::eTX_METHOD_ON_EVENT:
      c_ToolTipContent +=
         static_cast<QString>(static_cast<QString>("   ") + C_GtGetText::h_GetText("Tx-Method: On Event\n"));
      break;
   case C_OSCCanMessage::eTX_METHOD_CYCLIC:
      c_ToolTipContent +=
         static_cast<QString>(static_cast<QString>("   ") + C_GtGetText::h_GetText("Tx-Method: Cyclic\n"));
      c_ToolTipContent +=
         static_cast<QString>(static_cast<QString>("   ") + C_GtGetText::h_GetText("Cycle Time: %1\n")).arg(
            orc_Message.u32_CycleTimeMs);
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_CHANGE:
      c_ToolTipContent +=
         static_cast<QString>(static_cast<QString>("   ") + C_GtGetText::h_GetText("Tx-Method: On Change\n"));
      c_ToolTipContent +=
         static_cast<QString>(static_cast<QString>("   ") + C_GtGetText::h_GetText("Not earlier than: %1\n")).arg(
            orc_Message.u16_DelayTimeMs);
      c_ToolTipContent +=
         static_cast<QString>(static_cast<QString>("   ") +
                              C_GtGetText::h_GetText("But not later than: %1\n")).arg(orc_Message.u32_CycleTimeMs);
      break;
   default:
      tgl_assert(false);
      break;
   }
   return c_ToolTipContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for tooltip content for signal.

   \param[in]     orc_Signal                   Signal information for which the tooltip is requested
   \param[in]     orc_Message                  Message information
   \param[in]     orc_DpListElement            Signal information datapool part
   \param[in]     orc_AutoMinMaxInformation    Optional auto min max information string which will get inserted
                                               after the signal properties heading (no "\n" at end necessary)
   \param[in]     orc_AdditionalInformation    Optional additional information string which will get inserted after comment
                                               (no "\n" at end necessary)

   \return
      content as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdTooltipUtil::h_GetToolTipContentSignal(const C_OSCCanSignal & orc_Signal,
                                                   const C_OSCCanMessage & orc_Message,
                                                   const C_OSCNodeDataPoolListElement & orc_DpListElement,
                                                   const QString & orc_AutoMinMaxInformation,
                                                   const QString & orc_AdditionalInformation)
{
   QString c_ToolTipContent = "";
   float64 f64_Value = 0.0;

   // Comment
   if (orc_DpListElement.c_Comment.IsEmpty() == false)
   {
      c_ToolTipContent = orc_DpListElement.c_Comment.c_str();
      c_ToolTipContent.append("\n\n");
   }

   //additional info
   if (orc_AdditionalInformation.isEmpty() == false)
   {
      c_ToolTipContent.append(orc_AdditionalInformation);
      c_ToolTipContent.append("\n\n");
   }

   // Value information
   c_ToolTipContent.append(C_GtGetText::h_GetText("Signal Properties: \n"));
   if (orc_AutoMinMaxInformation.isEmpty() == false)
   {
      c_ToolTipContent.append(orc_AutoMinMaxInformation);
      c_ToolTipContent.append("\n");
   }

   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Min: "));
   if (C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_DpListElement.c_MinValue, f64_Value, 0UL) == C_NO_ERR)
   {
      c_ToolTipContent.append(QString::number(f64_Value));
   }
   c_ToolTipContent.append("\n");

   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Max: "));
   if (C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_DpListElement.c_MaxValue, f64_Value, 0UL) == C_NO_ERR)
   {
      c_ToolTipContent.append(QString::number(f64_Value));
   }
   c_ToolTipContent.append("\n");

   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Factor: "));
   c_ToolTipContent.append(QString::number(orc_DpListElement.f64_Factor));
   c_ToolTipContent.append("\n");

   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Offset: "));
   c_ToolTipContent.append(QString::number(orc_DpListElement.f64_Offset));
   c_ToolTipContent.append("\n");

   if (orc_DpListElement.c_DataSetValues.size() > 0UL)
   {
      c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Init Value: "));
      if (C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_DpListElement.c_DataSetValues[0UL],
                                                    f64_Value, 0UL) == C_NO_ERR)
      {
         c_ToolTipContent.append(QString::number(f64_Value));
      }
   }

   if (orc_DpListElement.c_Unit.IsEmpty() == false)
   {
      c_ToolTipContent.append("\n");
      c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Unit: "));
      c_ToolTipContent.append(orc_DpListElement.c_Unit.c_str());
   }
   c_ToolTipContent.append("\n");

   // Value type information
   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Byte Order: "));
   if (orc_Signal.e_ComByteOrder == C_OSCCanSignal::eBYTE_ORDER_INTEL)
   {
      c_ToolTipContent.append(C_GtGetText::h_GetText("Intel"));
   }
   else
   {
      c_ToolTipContent.append(C_GtGetText::h_GetText("Motorola"));
   }
   c_ToolTipContent.append("\n");

   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Type: "));
   c_ToolTipContent.append(C_SdTooltipUtil::h_ConvertTypeToNameSimplified(orc_DpListElement.c_MinValue.GetType()));
   c_ToolTipContent.append("\n");

   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Length: "));
   c_ToolTipContent.append(QString::number(orc_Signal.u16_ComBitLength));
   c_ToolTipContent.append(C_GtGetText::h_GetText(" Bit\n"));

   // Layout information
   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Message: "));
   c_ToolTipContent.append(orc_Message.c_Name.c_str());
   c_ToolTipContent.append("\n");

   c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Start Bit: "));
   c_ToolTipContent.append(QString::number(orc_Signal.u16_ComBitStart));

   if (orc_Signal.e_MultiplexerType != C_OSCCanSignal::eMUX_DEFAULT)
   {
      c_ToolTipContent.append("\n");
      c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Multiplexer Type: "));

      if (orc_Signal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
      {
         c_ToolTipContent.append(C_GtGetText::h_GetText("Multiplexed Signal"));
         c_ToolTipContent.append("\n");
         c_ToolTipContent.append(static_cast<QString>("   ") + C_GtGetText::h_GetText("Multiplexer Value: "));
         c_ToolTipContent.append(QString::number(orc_Signal.u16_MultiplexValue));
      }
      else
      {
         // Multiplexer signal
         c_ToolTipContent.append(C_GtGetText::h_GetText("Multiplexer Signal"));
      }
   }

   return c_ToolTipContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool content type to simplified name

   \param[in] ore_Type Node data pool content type

   \return
   Simplified name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdTooltipUtil::h_ConvertTypeToNameSimplified(const C_OSCNodeDataPoolContent::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8:
   case C_OSCNodeDataPoolContent::eUINT16:
   case C_OSCNodeDataPoolContent::eUINT32:
   case C_OSCNodeDataPoolContent::eUINT64:
      c_Retval = C_GtGetText::h_GetText("unsigned");
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
   case C_OSCNodeDataPoolContent::eSINT16:
   case C_OSCNodeDataPoolContent::eSINT32:
   case C_OSCNodeDataPoolContent::eSINT64:
      c_Retval = C_GtGetText::h_GetText("signed");
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      c_Retval = C_GtGetText::h_GetText("IEEE float (32)");
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      c_Retval = C_GtGetText::h_GetText("IEEE float (64)");
      break;
   default:
      c_Retval = "";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTooltipUtil::C_SdTooltipUtil(void)
{
}
