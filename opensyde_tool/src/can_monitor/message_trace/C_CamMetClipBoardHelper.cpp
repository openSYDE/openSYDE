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

   \param[in] oq_DisplayAsHex             Flag for display style for CAN ID and CAN data
   \param[in] oq_DisplayTimestampRelative Flag for display style for timestamp
   \param[in] orc_MessageData             Vector with pointer to all message data and the flag if the
                                          signals should be visible too
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetClipBoardHelper::h_StoreCanMessages(const bool oq_DisplayAsHex, const bool oq_DisplayTimestampRelative,
                                                 const std::vector<C_CamMetClipBoardHelperCanMessageData> & orc_MessageData)
{
   QString c_Text = "   Time             ID          Name                   "
                    "Dir  DLC Data                              Counter\n";
   uint32 u32_CounterMessage;

   for (u32_CounterMessage = 0U; u32_CounterMessage < orc_MessageData.size(); ++u32_CounterMessage)
   {
      const C_OSCComMessageLoggerData * const pc_Data = orc_MessageData[u32_CounterMessage].pc_MessageData;

      if (pc_Data != NULL)
      {
         QString c_Line;
         sintn sn_LineLength;

         // Create tree symbolic
         if ((pc_Data->c_Signals.size() > 0))
         {
            if (orc_MessageData[u32_CounterMessage].q_Extended == true)
            {
               c_Line = "-  ";
            }
            else
            {
               c_Line = "+  ";
            }
         }
         else
         {
            c_Line = "   ";
         }

         // Timestamp dependent of the style configuration
         // Save the length of the string before adding the new information to have an easier way to
         // fill it up with spaces
         sn_LineLength = c_Line.length();
         if (oq_DisplayTimestampRelative == true)
         {
            c_Line += pc_Data->c_TimeStampRelative.c_str();
         }
         else
         {
            c_Line += pc_Data->c_TimeStampAbsolute.c_str();
         }
         c_Line = c_Line.leftJustified(sn_LineLength + 17, ' ');

         // CAN ID dependent of the style configuration
         sn_LineLength = c_Line.length();
         if (oq_DisplayAsHex == true)
         {
            c_Line += pc_Data->c_CanIdHex.c_str();
         }
         else
         {
            c_Line += pc_Data->c_CanIdDec.c_str();
         }
         c_Line = c_Line.leftJustified(sn_LineLength + 12, ' ');

         // Name
         sn_LineLength = c_Line.length();
         c_Line += pc_Data->c_Name.c_str();
         c_Line = c_Line.leftJustified(sn_LineLength + 20, ' ');
         // Name could be longer
         c_Line += "   ";

         // Direction
         sn_LineLength = c_Line.length();
         if (pc_Data->q_IsTx == true)
         {
            c_Line += C_GtGetText::h_GetText("Tx");
         }
         else
         {
            c_Line += C_GtGetText::h_GetText("Rx");
         }
         c_Line = c_Line.leftJustified(sn_LineLength + 5, ' ');

         // DLC
         sn_LineLength = c_Line.length();
         c_Line += pc_Data->c_CanDlc.c_str();
         c_Line = c_Line.leftJustified(sn_LineLength + 4, ' ');

         // Data
         sn_LineLength = c_Line.length();
         if (oq_DisplayAsHex == true)
         {
            c_Line += pc_Data->c_CanDataHex.c_str();
         }
         else
         {
            c_Line += pc_Data->c_CanDataDec.c_str();
         }
         c_Line = c_Line.leftJustified(sn_LineLength + 34, ' ');

         // Counter
         c_Line += pc_Data->c_Counter.c_str() + QString("\n");

         if (orc_MessageData[u32_CounterMessage].q_Extended == true)
         {
            // Signals
            uint32 u32_CounterSignal;

            for (u32_CounterSignal = 0U; u32_CounterSignal < pc_Data->c_Signals.size(); ++u32_CounterSignal)
            {
               QString c_LineSignal = "|- ";
               const C_OSCComMessageLoggerDataSignal & rc_Signal = pc_Data->c_Signals[u32_CounterSignal];

               // Name
               sn_LineLength = c_LineSignal.length();
               c_LineSignal += rc_Signal.c_Name.c_str();
               c_LineSignal = c_LineSignal.leftJustified(sn_LineLength + 30, ' ');
               // Name could be longer
               c_LineSignal += "   ";

               if (rc_Signal.q_DlcError == false)
               {
                  // Value
                  sn_LineLength = c_LineSignal.length();
                  c_LineSignal += rc_Signal.c_Value.c_str() + QString(" ") + rc_Signal.c_Unit.c_str();
                  c_LineSignal = c_LineSignal.leftJustified(sn_LineLength + 25, ' ');
                  // Name could be longer
                  c_LineSignal += "   ";

                  // RAW Value
                  c_LineSignal += "RAW: ";
                  sn_LineLength = c_LineSignal.length();
                  if (oq_DisplayAsHex == true)
                  {
                     c_LineSignal += rc_Signal.c_RawValueHex.c_str();
                  }
                  else
                  {
                     c_LineSignal += rc_Signal.c_RawValueDec.c_str();
                  }

                  if (rc_Signal.c_Comment != "")
                  {
                     c_LineSignal = c_LineSignal.leftJustified(sn_LineLength + 25, ' ');
                  }
               }
               else
               {
                  // DLC error, signal did not fit into CAN message
                  sn_LineLength = c_LineSignal.length();
                  c_LineSignal += C_GtGetText::h_GetText("DLC error detected");

                  if (rc_Signal.c_Comment != "")
                  {
                     c_LineSignal = c_LineSignal.rightJustified(sn_LineLength + 47, ' ');
                  }
               }

               // Comment
               c_LineSignal += rc_Signal.c_Comment.c_str() + QString("\n");

               c_Line += c_LineSignal;
            }
         }

         c_Text += c_Line;
      }
   }

   C_CamMetClipBoardHelper::mh_SetClipBoard(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetClipBoardHelper::C_CamMetClipBoardHelper(void) :
   C_UtiClipBoardHelper()
{
}
