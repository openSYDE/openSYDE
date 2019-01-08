//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handler class for logging operations (implementation)

   Handler class for logging operations

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include "TGLFile.h"
#include "C_OSCLoggingHandler.h"
#include "stwerrors.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
bool C_OSCLoggingHandler::mhq_WriteToFile = false;
bool C_OSCLoggingHandler::mhq_WriteToConsole = true;
C_SCLString C_OSCLoggingHandler::mhc_FileName = "";
C_TGLCriticalSection C_OSCLoggingHandler::mhc_ConsoleCriticalSection;
C_TGLCriticalSection C_OSCLoggingHandler::mhc_FileCriticalSection;
std::ofstream C_OSCLoggingHandler::mhc_File;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Set write to file active flag

   \param[in] oq_Active New write to file active flag

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::h_SetWriteToFileActive(const bool oq_Active)
{
   if (C_OSCLoggingHandler::mhc_File.is_open() == true)
   {
      C_OSCLoggingHandler::mhc_File.close();
   }
   C_OSCLoggingHandler::mhq_WriteToFile = oq_Active;
   mh_OpenFile();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set write to console active flag

   \param[in] oq_Active New write to console active flag

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::h_SetWriteToConsoleActive(const bool oq_Active)
{
   C_OSCLoggingHandler::mhq_WriteToConsole = oq_Active;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set log file location path and file name

   \param[in] orc_CompleteLogFileLocation Log file location path and file name

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::h_SetCompleteLogFileLocation(const C_SCLString & orc_CompleteLogFileLocation)
{
   if (C_OSCLoggingHandler::mhc_File.is_open() == true)
   {
      C_OSCLoggingHandler::mhc_File.close();
   }
   C_OSCLoggingHandler::mhc_FileName = orc_CompleteLogFileLocation;
   mh_OpenFile();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get complete log file location

   \return
   Current complete log file location

   \created     16.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_SCLString & C_OSCLoggingHandler::h_GetCompleteLogFileLocation(void)
{
   return C_OSCLoggingHandler::mhc_FileName;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write info message to log

   \param[in] orc_Activity Current activity
   \param[in] orc_Message  Message to write (No '\n' necessary)
   \param[in] opc_Class    Current class
   \param[in] opc_Function Current function

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::h_WriteLogInfo(const C_SCLString & orc_Activity, const C_SCLString & orc_Message,
                                         const stw_types::charn * const opc_Class,
                                         const stw_types::charn * const opc_Function)
{
   C_OSCLoggingHandler::mh_WriteLog("INFO", orc_Activity, orc_Message, opc_Class, opc_Function);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write warning message to log

   \param[in] orc_Activity Current activity
   \param[in] orc_Message  Message to write (No '\n' necessary)
   \param[in] opc_Class    Current class
   \param[in] opc_Function Current function

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::h_WriteLogWarning(const C_SCLString & orc_Activity, const C_SCLString & orc_Message,
                                            const stw_types::charn * const opc_Class,
                                            const stw_types::charn * const opc_Function)
{
   C_OSCLoggingHandler::mh_WriteLog("WARNING", orc_Activity, orc_Message, opc_Class, opc_Function);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write error message to log

   \param[in] orc_Activity Current activity
   \param[in] orc_Message  Message to write (No '\n' necessary)
   \param[in] opc_Class    Current class
   \param[in] opc_Function Current function

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::h_WriteLogError(const C_SCLString & orc_Activity, const C_SCLString & orc_Message,
                                          const stw_types::charn * const opc_Class,
                                          const stw_types::charn * const opc_Function)
{
   C_OSCLoggingHandler::mh_WriteLog("ERROR", orc_Activity, orc_Message, opc_Class, opc_Function);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get readable stw error

   \param[in] os32_Error STW error value

   \return
   STW error string

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCLoggingHandler::h_StwError(const sint32 os32_Error)
{
   C_SCLString c_Retval;

   switch (os32_Error)
   {
   case C_NO_ERR:
      c_Retval = "C_NO_ERR";
      break;
   case C_WARN:
      c_Retval = "C_WARN";
      break;
   case C_DEFAULT:
      c_Retval = "C_DEFAULT";
      break;
   case C_BUSY:
      c_Retval = "C_BUSY";
      break;
   case C_RANGE:
      c_Retval = "C_RANGE";
      break;
   case C_OVERFLOW:
      c_Retval = "C_OVERFLOW";
      break;
   case C_NOACT:
      c_Retval = "C_NOACT";
      break;
   case C_COM:
      c_Retval = "C_COM";
      break;
   case C_RD_WR:
      c_Retval = "C_RD_WR";
      break;
   case C_CONFIG:
      c_Retval = "C_CONFIG";
      break;
   case C_CHECKSUM:
      c_Retval = "C_CHECKSUM";
      break;
   case C_TIMEOUT:
      c_Retval = "C_TIMEOUT";
      break;
   case C_UNKNOWN_ERR:
   default:
      c_Retval = "C_UNKNOWN_ERR";
      break;
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility to flush current log entries (No manual call necessary)

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::h_Flush(void)
{
   if (C_OSCLoggingHandler::mhq_WriteToConsole == true)
   {
      //Critical section
      C_OSCLoggingHandler::mhc_ConsoleCriticalSection.Acquire();
      std::cout << &std::flush;
      //Critical section
      C_OSCLoggingHandler::mhc_ConsoleCriticalSection.Release();
   }

   //File
   if ((C_OSCLoggingHandler::mhq_WriteToFile == true) && (C_OSCLoggingHandler::mhc_File.is_open() == true))
   {
      //Critical section
      C_OSCLoggingHandler::mhc_FileCriticalSection.Acquire();
      C_OSCLoggingHandler::mhc_File.flush();
      //Critical section
      C_OSCLoggingHandler::mhc_FileCriticalSection.Release();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get date time as formatted string

   Format:2017-08-29 07:32:19.123

   \param[in] orc_DateTime Date time structure

   \return
   Formatted string

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::string C_OSCLoggingHandler::h_UtilConvertDateTimeToString(const C_TGLDateTime & orc_DateTime)
{
   std::stringstream c_Stream;

   c_Stream << &std::right << std::setw(4) << std::setfill('0') << orc_DateTime.mu16_Year << "-";
   c_Stream << &std::right << std::setw(2) << std::setfill('0') << static_cast<uint16>(orc_DateTime.mu8_Month) << "-";
   c_Stream << &std::right << std::setw(2) << std::setfill('0') << static_cast<uint16>(orc_DateTime.mu8_Day) << " ";
   c_Stream << &std::right << std::setw(2) << std::setfill('0') << static_cast<uint16>(orc_DateTime.mu8_Hour) << ":";
   c_Stream << &std::right << std::setw(2) << std::setfill('0') << static_cast<uint16>(orc_DateTime.mu8_Minute) << ":";
   c_Stream << &std::right << std::setw(2) << std::setfill('0') << static_cast<uint16>(orc_DateTime.mu8_Second) << ".";
   c_Stream << &std::right << std::setw(3) << std::setfill('0') << orc_DateTime.mu16_MilliSeconds;
   return c_Stream.str();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write message to log

   \param[in] oe_Type      Message type (maximum 7 characters)
   \param[in] orc_Activity Current activity (maximum 26 characters)
   \param[in] orc_Message  Message to write
   \param[in] opc_Class    Current class (combined with function: maximum 50 characters)
                           May be file path string which is reduced to file base name automatically
   \param[in] opc_Function Current function (combined with function: maximum 50 characters)

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::mh_WriteLog(const C_SCLString & orc_Type, const C_SCLString & orc_Activity,
                                      const C_SCLString & orc_Message, const stw_types::charn * const opc_Class,
                                      const stw_types::charn * const opc_Function)
{
   std::string c_DateTimeFormatted;
   C_TGLDateTime c_DateTime;
   std::stringstream c_LogEntryStream;
   C_SCLString c_Class;
   C_SCLString c_Function;
   C_SCLString c_CombinedClassAndFunction;
   if (opc_Class != NULL)
   {
      c_Class = TGL_ChangeFileExtension(TGL_ExtractFileName(opc_Class), "");
   }
   else
   {
      c_Class = "UNKNOWN_CLASS";
   }
   if (opc_Function != NULL)
   {
      c_Function = opc_Function;
   }
   else
   {
      c_Function = "UNKNOWN_FUNCTION";
   }
   c_CombinedClassAndFunction = c_Class + "::" + c_Function;

   TGL_GetDateTimeNow(c_DateTime);
   c_DateTimeFormatted = C_OSCLoggingHandler::h_UtilConvertDateTimeToString(c_DateTime);

   //Format:
   //[DATE/TIME] [TYPE_OF_REPORT (Info, Warning, Error)] [ACTIVITY] [CLASS::FUNCTION] [MESSAGE]
   //2017-08-29 07:32:19.123      INFO       Startup         Main                            Application started.
   c_LogEntryStream << &std::left << std::setw(25) << c_DateTimeFormatted;
   c_LogEntryStream << &std::left << std::setw(7) << orc_Type.c_str();
   c_LogEntryStream << "  ";
   c_LogEntryStream << &std::left << std::setw(26) << orc_Activity.c_str();
   c_LogEntryStream << "  ";
   c_LogEntryStream << &std::left << std::setw(52) << c_CombinedClassAndFunction.c_str();
   c_LogEntryStream << "  ";
   c_LogEntryStream << &std::left << orc_Message.c_str() << &std::endl;

   //Console
   if (C_OSCLoggingHandler::mhq_WriteToConsole == true)
   {
      //Critical section
      C_OSCLoggingHandler::mhc_ConsoleCriticalSection.Acquire();
      std::cout << c_LogEntryStream.str();
      //Critical section
      C_OSCLoggingHandler::mhc_ConsoleCriticalSection.Release();
   }

   //File
   if ((C_OSCLoggingHandler::mhq_WriteToFile == true) && (C_OSCLoggingHandler::mhc_File.is_open() == true))
   {
      const std::string c_Message = c_LogEntryStream.str();
      //Critical section
      C_OSCLoggingHandler::mhc_FileCriticalSection.Acquire();

      //TGL critical section -> file
      C_OSCLoggingHandler::mhc_File.write(c_Message.c_str(), c_Message.size());

      //Critical section
      C_OSCLoggingHandler::mhc_FileCriticalSection.Release();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Open file if necessary

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCLoggingHandler::mh_OpenFile(void)
{
   if (((C_OSCLoggingHandler::mhc_FileName != "") && (C_OSCLoggingHandler::mhq_WriteToFile == true)) &&
       (C_OSCLoggingHandler::mhc_File.is_open() == false))
   {
      const C_SCLString c_FilePath = TGL_ExtractFilePath(C_OSCLoggingHandler::mhc_FileName);
      //Folder
      if (TGL_DirectoryExists(c_FilePath) == false)
      {
         TGL_CreateDirectory(c_FilePath);
      }
      C_OSCLoggingHandler::mhc_File.open(C_OSCLoggingHandler::mhc_FileName.c_str(), std::ios::app);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCLoggingHandler::C_OSCLoggingHandler(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCLoggingHandler::~C_OSCLoggingHandler(void)
{
   if (C_OSCLoggingHandler::mhc_File.is_open() == true)
   {
      C_OSCLoggingHandler::mhc_File.close();
   }
}
