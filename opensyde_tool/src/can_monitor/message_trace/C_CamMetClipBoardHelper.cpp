//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to copy a message and its detected signals to the clipboard as text (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GtGetText.h"

#include "C_CamMetUtil.h"
#include "C_CamMetClipBoardHelper.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copies the CAN message in text form to the clipboard

   Style of copied information:
   - Without name
   - With name
   - With signals collapsed
   - With signals expanded

      Time     ID       Name           Dir      DLC      Data                       Counter
      0.123    1FFFFFFF                Rx       8        00 07 00 0C 00 0D 00 0C    10
      0.123    1FFFFFFF MessageName    Rx       8        00 07 00 0C 00 0D 00 0C    10
   +  0.123    1FFFFFFF MessageName    Rx       8        00 07 00 0C 00 0D 00 0C    10
   +  0.123    1FFFFFFF MessageName    Rx       8        00 07 00 0C 00 0D 00 0C    10
   |- SignalName        200 Unit     C8 (RAW)   Comment
   |- NextSignalName    200 Unit     C8 (RAW)   Comment

   \param[in]  oq_DisplayAsHex                        Flag for display style for CAN ID and CAN data
   \param[in]  oq_DisplayTimestampRelative            Flag for display style for timestamp
   \param[in]  oq_DisplayTimestampAbsoluteTimeOfDay   Flag for display style for absolute timestamp. Not relevant if
                                                      oq_DisplayTimestampRelative is true
                                                      true: Timestamp with time of day
                                                      false: Timestamp beginning at start of measurement
   \param[in]  orc_MessageData                        Vector with pointer to all message data and the flag if the
                                                      signals should be visible too
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetClipBoardHelper::h_StoreCanMessages(const bool oq_DisplayAsHex, const bool oq_DisplayTimestampRelative,
                                                 const bool oq_DisplayTimestampAbsoluteTimeOfDay,
                                                 const std::vector<C_CamMetClipBoardHelperCanMessageData> & orc_MessageData)
{
   const sintn sn_WidthInitial = 4;
   const sintn sn_WidthTime = 25;
   const sintn sn_WidthID = 12;
   const sintn sn_WidthName = 40;
   const sintn sn_WidthDir = 12;
   const sintn sn_WidthDLC = 12;
   const sintn sn_WidthData = 34;
   QString c_Text;
   uint32 u32_CounterMessage;

   //Build header
   C_CamMetClipBoardHelper::h_AddHeader(c_Text, sn_WidthInitial, sn_WidthTime, sn_WidthID, sn_WidthName, sn_WidthDir,
                                        sn_WidthDLC, sn_WidthData);

   for (u32_CounterMessage = 0U; u32_CounterMessage < orc_MessageData.size(); ++u32_CounterMessage)
   {
      const C_OSCComMessageLoggerData * const pc_Data = orc_MessageData[u32_CounterMessage].pc_MessageData;

      if (pc_Data != NULL)
      {
         C_CamMetClipBoardHelper::h_AddMessage(c_Text, *pc_Data, orc_MessageData[u32_CounterMessage].q_Extended,
                                               oq_DisplayAsHex, oq_DisplayTimestampRelative,
                                               oq_DisplayTimestampAbsoluteTimeOfDay, sn_WidthInitial, sn_WidthTime,
                                               sn_WidthID, sn_WidthName, sn_WidthDir, sn_WidthDLC, sn_WidthData,
                                               orc_MessageData[u32_CounterMessage].c_ExpandedIndices);
      }
   }

   C_CamMetClipBoardHelper::mh_SetClipBoard(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add header

   \param[in,out]  orc_Text            Text
   \param[in]      osn_WidthInitial    Width initial section
   \param[in]      osn_WidthTime       Width time section
   \param[in]      osn_WidthID         Width ID section
   \param[in]      osn_WidthName       Width name section
   \param[in]      osn_WidthDir        Width direction section
   \param[in]      osn_WidthDLC        Width DLC section
   \param[in]      osn_WidthData       Width data section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetClipBoardHelper::h_AddHeader(QString & orc_Text, const sintn osn_WidthInitial, const sintn osn_WidthTime,
                                          const sintn osn_WidthID, const sintn osn_WidthName, const sintn osn_WidthDir,
                                          const sintn osn_WidthDLC, const sintn osn_WidthData)
{
   sintn sn_TextLength;

   orc_Text = " ";
   orc_Text = orc_Text.leftJustified(osn_WidthInitial, ' ');

   sn_TextLength = orc_Text.length();
   orc_Text += C_GtGetText::h_GetText("Time");
   orc_Text = orc_Text.leftJustified(sn_TextLength + osn_WidthTime, ' ');

   sn_TextLength = orc_Text.length();
   orc_Text += C_GtGetText::h_GetText("ID");
   orc_Text = orc_Text.leftJustified(sn_TextLength + osn_WidthID, ' ');

   sn_TextLength = orc_Text.length();
   orc_Text += C_GtGetText::h_GetText("Name");
   orc_Text = orc_Text.leftJustified(sn_TextLength + osn_WidthName, ' ');

   sn_TextLength = orc_Text.length();
   orc_Text += C_GtGetText::h_GetText("Dir");
   orc_Text = orc_Text.leftJustified(sn_TextLength + osn_WidthDir, ' ');

   sn_TextLength = orc_Text.length();
   orc_Text += C_GtGetText::h_GetText("DLC");
   orc_Text = orc_Text.leftJustified(sn_TextLength + osn_WidthDLC, ' ');

   sn_TextLength = orc_Text.length();
   orc_Text += C_GtGetText::h_GetText("Data");
   orc_Text = orc_Text.leftJustified(sn_TextLength + osn_WidthData, ' ');

   orc_Text += C_GtGetText::h_GetText("Counter");
   orc_Text += "\n";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add message

   \param[in,out]  orc_Text                              Text
   \param[in]      orc_MessageData                       Message data
   \param[in]      oq_IsExtended                         Message is extended
   \param[in]      oq_DisplayAsHex                       Flag for display style for CAN ID and CAN data
   \param[in]      oq_DisplayTimestampRelative           Flag for display style for timestamp
   \param[in]      oq_DisplayTimestampAbsoluteTimeOfDay  Flag for display style for absolute timestamp. Not relevant if
                                                         oq_DisplayTimestampRelative is true
                                                         true: Timestamp with time of day
                                                         false: Timestamp beginning at start of measurement
   \param[in]      osn_WidthInitial                      Width initial section
   \param[in]      osn_WidthTime                         Width time section
   \param[in]      osn_WidthID                           Width ID section
   \param[in]      osn_WidthName                         Width name section
   \param[in]      osn_WidthDir                          Width direction section
   \param[in]      osn_WidthDLC                          Width DLC section
   \param[in]      osn_WidthData                         Width data section
   \param[in]      orc_ExpandedIndices                   Expanded indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetClipBoardHelper::h_AddMessage(QString & orc_Text, const C_OSCComMessageLoggerData & orc_MessageData,
                                           const bool oq_IsExtended, const bool oq_DisplayAsHex,
                                           const bool oq_DisplayTimestampRelative,
                                           const bool oq_DisplayTimestampAbsoluteTimeOfDay,
                                           const sintn osn_WidthInitial, const sintn osn_WidthTime,
                                           const sintn osn_WidthID, const sintn osn_WidthName, const sintn osn_WidthDir,
                                           const sintn osn_WidthDLC, const sintn osn_WidthData,
                                           const std::vector<sintn> & orc_ExpandedIndices)
{
   QString c_Line;
   sintn sn_LineLength;

   // Create tree symbolic
   if ((orc_MessageData.c_Signals.size() > 0))
   {
      if (oq_IsExtended == true)
      {
         c_Line = "-";
      }
      else
      {
         c_Line = "+";
      }
   }
   else
   {
      c_Line = " ";
   }
   c_Line = c_Line.leftJustified(osn_WidthInitial, ' ');

   // Timestamp dependent of the style configuration
   // Save the length of the string before adding the new information to have an easier way to
   // fill it up with spaces
   sn_LineLength = c_Line.length();
   if (oq_DisplayTimestampRelative == true)
   {
      c_Line += orc_MessageData.c_TimeStampRelative.c_str();
   }
   else if (oq_DisplayTimestampAbsoluteTimeOfDay == false)
   {
      c_Line += orc_MessageData.c_TimeStampAbsoluteStart.c_str();
   }
   else
   {
      c_Line += orc_MessageData.c_TimeStampAbsoluteTimeOfDay.c_str();
   }
   c_Line = c_Line.leftJustified(sn_LineLength + osn_WidthTime, ' ');

   // CAN ID dependent of the style configuration
   sn_LineLength = c_Line.length();
   if (oq_DisplayAsHex == true)
   {
      c_Line += orc_MessageData.c_CanIdHex.c_str();
   }
   else
   {
      c_Line += orc_MessageData.c_CanIdDec.c_str();
   }
   c_Line = c_Line.leftJustified(sn_LineLength + osn_WidthID, ' ');

   // Name
   sn_LineLength = c_Line.length();
   c_Line += orc_MessageData.c_Name.c_str();
   // Name could be longer
   c_Line += "   ";
   c_Line = c_Line.leftJustified(sn_LineLength + osn_WidthName, ' ');

   // Direction
   sn_LineLength = c_Line.length();
   if (orc_MessageData.q_IsTx == true)
   {
      c_Line += C_GtGetText::h_GetText("Tx");
   }
   else
   {
      c_Line += C_GtGetText::h_GetText("Rx");
   }
   if (orc_MessageData.c_CanMsg.u8_RTR != 0)
   {
      c_Line += C_GtGetText::h_GetText(" (RTR)");
   }
   c_Line = c_Line.leftJustified(sn_LineLength + osn_WidthDir, ' ');

   // DLC
   sn_LineLength = c_Line.length();
   c_Line += orc_MessageData.c_CanDlc.c_str();
   if (orc_MessageData.q_CanDlcError)
   {
      c_Line += C_GtGetText::h_GetText(" (!= DB)");
   }
   c_Line = c_Line.leftJustified(sn_LineLength + osn_WidthDLC, ' ');

   // Data
   sn_LineLength = c_Line.length();
   if (orc_MessageData.c_CanMsg.u8_RTR == 0)
   {
      if (oq_DisplayAsHex == true)
      {
         if (orc_MessageData.c_ProtocolTextHex != "")
         {
            c_Line += orc_MessageData.c_ProtocolTextHex.c_str();
            //Always add at least one space
            c_Line += " ";
         }
         else
         {
            c_Line += orc_MessageData.c_CanDataHex.c_str();
         }
      }
      else
      {
         if (orc_MessageData.c_ProtocolTextDec != "")
         {
            c_Line += orc_MessageData.c_ProtocolTextDec.c_str();
            //Always add at least one space
            c_Line += " ";
         }
         else
         {
            c_Line += orc_MessageData.c_CanDataDec.c_str();
         }
      }
   }
   c_Line = c_Line.leftJustified(sn_LineLength + osn_WidthData, ' ');

   // Counter
   c_Line += orc_MessageData.c_Counter.c_str() + QString("\n");

   if (oq_IsExtended == true)
   {
      C_CamMetClipBoardHelper::h_AddCanSignals(osn_WidthInitial, oq_DisplayAsHex, orc_MessageData.c_Signals,
                                               orc_ExpandedIndices, c_Line);
   }

   orc_Text += c_Line;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add can signals to complete string

   \param[in]      osn_LineWidthInitial         Line width of initial section
   \param[in]      oq_DisplayAsHex              Flag for display style for CAN ID and CAN data
   \param[in]      orc_Signals                  Signals
   \param[in]      orc_ExpandedSignalIndices    Expanded signal indices
   \param[in,out]  orc_CompleteString           Complete string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetClipBoardHelper::h_AddCanSignals(const sintn osn_LineWidthInitial, const bool oq_DisplayAsHex,
                                              const std::vector<C_OSCComMessageLoggerDataSignal> & orc_Signals,
                                              const std::vector<sintn> & orc_ExpandedSignalIndices,
                                              QString & orc_CompleteString)
{
   uint32 u32_MultiplexerCounter = 0UL;

   //Determine order
   std::vector<stw_types::uint32> c_RelevantSignalsOrder;
   const std::vector<stw_types::sintn> c_MultiplexerOrder = C_CamMetUtil::h_GetMultiplexerOrder(orc_Signals);

   //Multiplexer
   for (uint32 u32_Order = 0U; u32_Order < c_MultiplexerOrder.size(); ++u32_Order)
   {
      for (uint32 u32_CounterSignal = 0U; u32_CounterSignal < orc_Signals.size(); ++u32_CounterSignal)
      {
         const C_OSCComMessageLoggerDataSignal & rc_Signal = orc_Signals[u32_CounterSignal];
         //Keep order
         if (rc_Signal.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
         {
            //Get value
            const sintn sn_MultiplexerValue = rc_Signal.c_RawValueDec.ToInt();
            if (c_MultiplexerOrder[u32_Order] == sn_MultiplexerValue)
            {
               bool q_Skip = true;
               //Add multiplexer
               c_RelevantSignalsOrder.push_back(u32_CounterSignal);
               //Children
               //See if parent multiplexer is expanded
               for (uint32 u32_ItExpanded = 0UL; (u32_ItExpanded < orc_ExpandedSignalIndices.size()) && (q_Skip);
                    ++u32_ItExpanded)
               {
                  if (orc_ExpandedSignalIndices[u32_ItExpanded] == static_cast<sintn>(u32_MultiplexerCounter))
                  {
                     q_Skip = false;
                  }
               }
               if (q_Skip == false)
               {
                  //Multiplexed
                  for (uint32 u32_ItSig = 0U; u32_ItSig < orc_Signals.size(); ++u32_ItSig)
                  {
                     const C_OSCComMessageLoggerDataSignal & rc_Sig = orc_Signals[u32_ItSig];
                     if (rc_Sig.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
                     {
                        if (static_cast<sintn>(rc_Sig.c_OscSignal.u16_MultiplexValue) == sn_MultiplexerValue)
                        {
                           //Add multiplexed
                           c_RelevantSignalsOrder.push_back(u32_ItSig);
                        }
                     }
                  }
               }
               //Iterate
               ++u32_MultiplexerCounter;
            }
         }
      }
   }
   //Others
   for (uint32 u32_CounterSignal = 0U; u32_CounterSignal < orc_Signals.size(); ++u32_CounterSignal)
   {
      const C_OSCComMessageLoggerDataSignal & rc_Signal = orc_Signals[u32_CounterSignal];
      if (rc_Signal.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_DEFAULT)
      {
         c_RelevantSignalsOrder.push_back(u32_CounterSignal);
      }
   }
   //Display
   // Signals
   for (uint32 u32_CounterSignal = 0U; u32_CounterSignal < c_RelevantSignalsOrder.size(); ++u32_CounterSignal)
   {
      const C_OSCComMessageLoggerDataSignal & rc_Signal = orc_Signals[c_RelevantSignalsOrder[u32_CounterSignal]];
      bool q_IsMuxSig;
      //Check if child is displayed
      if (rc_Signal.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
      {
         q_IsMuxSig = true;
      }
      else
      {
         q_IsMuxSig = false;
      }
      C_CamMetClipBoardHelper::h_AddCanSignal(osn_LineWidthInitial, oq_DisplayAsHex, rc_Signal,
                                              orc_CompleteString,
                                              q_IsMuxSig);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add can signal

   \param[in]      osn_LineWidthInitial   Line width initial
   \param[in]      oq_DisplayAsHex        Display as hex
   \param[in]      orc_Signal             Signal
   \param[in,out]  orc_CompleteString     Complete string
   \param[in]      oq_IsMux               Is mux
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetClipBoardHelper::h_AddCanSignal(const sintn osn_LineWidthInitial, const bool oq_DisplayAsHex,
                                             const C_OSCComMessageLoggerDataSignal & orc_Signal,
                                             QString & orc_CompleteString, const bool oq_IsMux)
{
   QString c_LineSignal;

   if (oq_IsMux)
   {
      //third level
      c_LineSignal = " |-";
   }
   else
   {
      //second level
      c_LineSignal = "|-";
   }
   c_LineSignal = c_LineSignal.leftJustified(osn_LineWidthInitial, ' ');

   // Name
   sintn sn_LineLength = c_LineSignal.length();
   c_LineSignal += orc_Signal.c_Name.c_str();
   c_LineSignal = c_LineSignal.leftJustified(sn_LineLength + 30, ' ');
   // Name could be longer
   c_LineSignal += "   ";

   if (orc_Signal.q_DlcError == false)
   {
      // Value
      sn_LineLength = c_LineSignal.length();
      c_LineSignal += orc_Signal.c_Value.c_str() + QString(" ") + orc_Signal.c_Unit.c_str();
      c_LineSignal = c_LineSignal.leftJustified(sn_LineLength + 25, ' ');
      // Name could be longer
      c_LineSignal += "   ";

      // RAW Value
      c_LineSignal += "RAW: ";
      sn_LineLength = c_LineSignal.length();
      if (oq_DisplayAsHex == true)
      {
         c_LineSignal += orc_Signal.c_RawValueHex.c_str();
      }
      else
      {
         c_LineSignal += orc_Signal.c_RawValueDec.c_str();
      }

      if (orc_Signal.c_Comment != "")
      {
         c_LineSignal = c_LineSignal.leftJustified(sn_LineLength + 25, ' ');
      }
   }
   else
   {
      // DLC error, signal did not fit into CAN message
      sn_LineLength = c_LineSignal.length();
      c_LineSignal += C_GtGetText::h_GetText("DLC error detected");

      if (orc_Signal.c_Comment != "")
      {
         c_LineSignal = c_LineSignal.rightJustified(sn_LineLength + 47, ' ');
      }
   }

   // Comment
   c_LineSignal += orc_Signal.c_Comment.c_str() + QString("\n");

   orc_CompleteString += c_LineSignal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetClipBoardHelper::C_CamMetClipBoardHelper(void) :
   C_UtiClipBoardHelper()
{
}
