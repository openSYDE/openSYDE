//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with generic interface to handle and write a CAN log file (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

//we do not use the unspecified values; just "pass through" to get the clearly defined structure "struct tm"
//lint -estring(829,ctime)
//lint -estring(586,time)
//lint -estring(586,localtime)
#include <ctime>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscComMessageLoggerFileAsc.hpp"
#include "TglFile.hpp"
#include "TglTime.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_FilePath                 Path for file
   \param[in]  orc_ProtocolName             Name of current set protocol
   \param[in]  oq_HexActive                 Mode for writing CAN Id and CAN data (hexadecimal or decimal)
   \param[in]  oq_RelativeTimeStampActive   Mode for writing CAN timestamp (relative or absolute)
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLoggerFileAsc::C_OscComMessageLoggerFileAsc(const C_SclString & orc_FilePath,
                                                           const C_SclString & orc_ProtocolName,
                                                           const bool oq_HexActive,
                                                           const bool oq_RelativeTimeStampActive) :
   C_OscComMessageLoggerFileBase(orc_FilePath, orc_ProtocolName),
   mq_HexActive(oq_HexActive),
   mq_RelativeTimeStampActive(oq_RelativeTimeStampActive)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Writes the end line and closes the open file
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLoggerFileAsc::~C_OscComMessageLoggerFileAsc(void)
{
   try
   {
      if (this->mc_File.is_open() == true)
      {
         const C_SclString c_EndLine = "End TriggerBlock";
         this->mc_File.write(c_EndLine.c_str(), c_EndLine.Length());
         this->mc_File.close();
      }
   }
   catch (...)
   {
      //not much we can do here ...
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates, if necessary, and opens file and adds the default header of the file.

   An already opened file will be closed and deleted.

   \return
   C_NO_ERR    File successfully opened and created
   C_RD_WR     Error on creating file, folders or deleting old file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLoggerFileAsc::OpenFile(void)
{
   int32_t s32_Return;

   if (this->mc_File.is_open() == true)
   {
      // Close the file if it is open. The previous file will be deleted
      this->mc_File.close();
   }

   if (this->mc_FilePath.SubString(this->mc_FilePath.Length() - 3U, 4U).LowerCase() != ".asc")
   {
      // Missing file extension
      this->mc_FilePath += ".asc";
   }

   s32_Return = C_OscComMessageLoggerFileBase::OpenFile();

   if (s32_Return == C_NO_ERR)
   {
      this->mc_File.open(this->mc_FilePath.c_str(), std::ios::app);

      // Write default header
      this->m_WriteHeader();

      // Check if the file was really created
      if (TglFileExists(this->mc_FilePath) == false)
      {
         // File was not created
         s32_Return = C_RD_WR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adding of a concrete CAN message to the log file

   \param[in]     orc_MessageData      Current CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLoggerFileAsc::AddMessageToFile(const C_OscComMessageLoggerData & orc_MessageData)
{
   if (this->mc_File.is_open() == true)
   {
      uint32_t u32_SignalCounter;
      C_SclString c_LogEntry = "   ";
      C_SclString c_Temp;

      // Timestamp
      if (this->mq_RelativeTimeStampActive == true)
      {
         c_LogEntry += mh_AdaptTimeStamp(orc_MessageData.u64_TimeStampRelative);
      }
      else
      {
         c_LogEntry += mh_AdaptTimeStamp(orc_MessageData.u64_TimeStampAbsoluteStart);
      }

      // Channel
      c_LogEntry += " 1 ";

      // CAN Id
      // ASC specification defines a width of fixed 15 chars for CAN Id. Plus one for the space to the direction
      if (this->mq_HexActive == true)
      {
         c_Temp.PrintFormatted("%-16s", orc_MessageData.c_CanIdHex.c_str());
      }
      else
      {
         c_Temp.PrintFormatted("%-16s", orc_MessageData.c_CanIdDec.c_str());
      }
      c_LogEntry += c_Temp;

      // Direction
      // ASC specification defines a width of fixed 4 chars for direction. Plus one for the space to the next section
      if (orc_MessageData.q_IsTx == false)
      {
         c_LogEntry += "Rx   ";
      }
      else
      {
         c_LogEntry += "Tx   ";
      }

      // Special case RTR
      if (orc_MessageData.c_CanMsg.u8_RTR > 0U)
      {
         c_LogEntry += "r";
      }
      else
      {
         // DLC and data
         c_LogEntry += "d " + orc_MessageData.c_CanDlc + " ";
         if (this->mq_HexActive == true)
         {
            c_LogEntry += orc_MessageData.c_CanDataHex;
         }
         else
         {
            c_LogEntry += orc_MessageData.c_CanDataDec;
         }
      }

      // State for specified message flags are not known here
      c_LogEntry += "\n";

      // Protocol interpretation
      if (orc_MessageData.c_ProtocolTextDec != "")
      {
         c_LogEntry += "   //" + this->mc_ProtocolName + " ";
         if (this->mq_HexActive == true)
         {
            c_LogEntry += orc_MessageData.c_ProtocolTextHex;
         }
         else
         {
            c_LogEntry += orc_MessageData.c_ProtocolTextDec;
         }
         c_LogEntry += "\n";
      }

      // Message information
      if (orc_MessageData.c_Name != "")
      {
         c_LogEntry += "   //Message  " + orc_MessageData.c_Name;

         if (orc_MessageData.q_CanDlcError == true)
         {
            c_LogEntry += " (DLC error)";
         }
         c_LogEntry += "\n";
      }

      // Detected signals
      for (u32_SignalCounter = 0U; u32_SignalCounter < orc_MessageData.c_Signals.size(); u32_SignalCounter++)
      {
         const C_OscComMessageLoggerDataSignal & rc_Signal = orc_MessageData.c_Signals[u32_SignalCounter];
         c_LogEntry += "   //Signal   " + rc_Signal.c_Name + " = " + rc_Signal.c_Value;
         if (rc_Signal.c_Unit != "")
         {
            c_LogEntry += " " + rc_Signal.c_Unit;
         }
         c_LogEntry += "\n";
      }

      this->mc_File.write(c_LogEntry.c_str(), c_LogEntry.Length());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Writes header of asc log file

   Format:
   date <WeekDay> <Month> <Date> <Fulltime> <Year>
   base <hex|dec> timestamps <absolute|relative>
   no internal events logged
   // version 7.2.0
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLoggerFileAsc::m_WriteHeader(void)
{
   if (this->mc_File.is_open() == true)
   {
      C_SclString c_Header;
      const C_SclString c_TimeString = mh_GetAscTimeString();

      // First line is date and time
      c_Header = "date " + c_TimeString + "\n";

      // Second line is the configuration
      c_Header += "base ";
      if (this->mq_HexActive == true)
      {
         c_Header += "hex ";
      }
      else
      {
         c_Header += "dec ";
      }

      c_Header += "timestamps ";
      if (this->mq_RelativeTimeStampActive == true)
      {
         c_Header += "relative\n";
      }
      else
      {
         c_Header += "absolute\n";
      }

      // Line with information about event logging
      c_Header += "no internal events logged\n";

      // version
      c_Header += "// version 7.2.0\n";
      c_Header += "Begin Triggerblock " + c_TimeString + "\n";

      this->mc_File.write(c_Header.c_str(), c_Header.Length());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current date and time in the asc format

   \return
   Time and date in defined format
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLoggerFileAsc::mh_GetAscTimeString(void)
{
   C_SclString c_Result;
   C_SclString c_Temp;

   // Getting weekday
   const std::time_t x_Time = std::time(NULL);      //lint !e8080 using type expected by the library for compatibility
   const std::tm c_Time = *std::localtime(&x_Time); //lint !e613 //documentation of localtime says "not NULL"
   const uint32_t u32_TimeMs = TglGetTickCount();

   c_Result += mh_GetDay(c_Time.tm_wday) + " ";
   c_Result += mh_GetMonth(c_Time.tm_mon) + " ";
   c_Result += C_SclString::IntToStr(c_Time.tm_mday) + " ";
   // Hours
   c_Temp.PrintFormatted("%.2d", c_Time.tm_hour);
   c_Result += c_Temp + ":";
   // Minutes
   c_Temp.PrintFormatted("%.2d", c_Time.tm_min);
   c_Result += c_Temp + ":";
   // Seconds
   c_Temp.PrintFormatted("%.2d", c_Time.tm_sec);
   c_Result += c_Temp + ".";
   // Get the milliseconds
   c_Temp.PrintFormatted("%.3d", u32_TimeMs % 1000U);
   c_Result += c_Temp + " ";
   c_Result += C_SclString::IntToStr(1900 + c_Time.tm_year);

   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the weekday in the asc format

   \param[in]     os32_Day        Number for weekday, starting with Sunday a 0

   \return
   Weekday
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLoggerFileAsc::mh_GetDay(const int32_t os32_Day)
{
   C_SclString c_Day;

   switch (os32_Day)
   {
   case 0:
      c_Day = "Sun";
      break;
   case 1:
      c_Day = "Mon";
      break;
   case 2:
      c_Day = "Tue";
      break;
   case 3:
      c_Day = "Wed";
      break;
   case 4:
      c_Day = "Thu";
      break;
   case 5:
      c_Day = "Fri";
      break;
   case 6:
      c_Day = "Sat";
      break;
   default:
      // Should not happen
      c_Day = "Unk";
      break;
   }

   return c_Day;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the weekday in the asc format

   \param[in]     os32_Month       Number for month, starting with 0

   \return
   Weekday
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLoggerFileAsc::mh_GetMonth(const int32_t os32_Month)
{
   C_SclString c_Month;

   switch (os32_Month)
   {
   case 0:
      c_Month = "Jan";
      break;
   case 1:
      c_Month = "Feb";
      break;
   case 2:
      c_Month = "Mar";
      break;
   case 3:
      c_Month = "Apr";
      break;
   case 4:
      c_Month = "May";
      break;
   case 5:
      c_Month = "Jun";
      break;
   case 6:
      c_Month = "Jul";
      break;
   case 7:
      c_Month = "Aug";
      break;
   case 8:
      c_Month = "Sep";
      break;
   case 9:
      c_Month = "Oct";
      break;
   case 10:
      c_Month = "Nov";
      break;
   case 11:
      c_Month = "Dec";
      break;
   default:
      // Should not happen
      c_Month = "Unk";
      break;
   }

   return c_Month;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts a timestamp in a string in a format based on seconds

   Examples for format:
   " 0.000001"
   "10.000000"
   "1000.00123"

   \param[in]  ou64_TimeStamp     Timestamp in us

   \return
   Adapted string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLoggerFileAsc::mh_AdaptTimeStamp(const uint64_t ou64_TimeStamp)
{
   C_SclString c_TimeStamp;

   c_TimeStamp.PrintFormatted("%d.%.6d", static_cast<int32_t>(ou64_TimeStamp / 1000000ULL),
                              static_cast<int32_t>(ou64_TimeStamp % 1000000ULL));
   c_TimeStamp.PrintFormatted("%9s", c_TimeStamp.c_str());

   return c_TimeStamp;
}
