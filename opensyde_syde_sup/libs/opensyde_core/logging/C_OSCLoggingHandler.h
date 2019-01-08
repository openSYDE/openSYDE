//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for logging operations (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCLOGGINGHANDLER_H
#define C_OSCLOGGINGHANDLER_H

/* -- Includes ------------------------------------------------------------- */
#include "CSCLString.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "TGLTasks.h"
#include <fstream>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */
//PC-Lint suppressions: function-like macro is the only way to get file,func,line information into the text
//lint -save -e1960
#define osc_write_log_info(activity, \
                           message) (stw_opensyde_core::C_OSCLoggingHandler::h_WriteLogInfo(activity, message, __FILE__, \
                                                                                            TGL_UTIL_FUNC_ID))
#define osc_write_log_warning(activity, \
                              message) (stw_opensyde_core::C_OSCLoggingHandler::h_WriteLogWarning(activity, message, \
                                                                                                  __FILE__, \
                                                                                                  TGL_UTIL_FUNC_ID))
#define osc_write_log_error(activity, \
                            message) (stw_opensyde_core::C_OSCLoggingHandler::h_WriteLogError(activity, message, \
                                                                                              __FILE__, \
                                                                                              TGL_UTIL_FUNC_ID))
//lint -restore

/* -- Types ---------------------------------------------------------------- */

///Handler class for logging operations
class C_OSCLoggingHandler
{
public:
   //Configuration
   static void h_SetWriteToFileActive(const bool oq_Active);
   static void h_SetWriteToConsoleActive(const bool oq_Active);
   static void h_SetCompleteLogFileLocation(const stw_scl::C_SCLString & orc_CompleteLogFileLocation);
   static const stw_scl::C_SCLString & h_GetCompleteLogFileLocation(void);

   //Logging call
   static void h_WriteLogInfo(const stw_scl::C_SCLString & orc_Activity, const stw_scl::C_SCLString & orc_Message,
                              const stw_types::charn * const opc_Class = NULL,
                              const stw_types::charn * const opc_Function = NULL);
   static void h_WriteLogWarning(const stw_scl::C_SCLString & orc_Activity, const stw_scl::C_SCLString & orc_Message,
                                 const stw_types::charn * const opc_Class = NULL,
                                 const stw_types::charn * const opc_Function = NULL);
   static void h_WriteLogError(const stw_scl::C_SCLString & orc_Activity, const stw_scl::C_SCLString & orc_Message,
                               const stw_types::charn * const opc_Class = NULL,
                               const stw_types::charn * const opc_Function = NULL);

   static stw_scl::C_SCLString h_StwError(const stw_types::sint32 os32_Error);

   //Utility functions
   static void h_Flush(void);
   static std::string h_UtilConvertDateTimeToString(const stw_tgl::C_TGLDateTime & orc_DateTime);

private:
   static bool mhq_WriteToFile;
   static bool mhq_WriteToConsole;
   static stw_scl::C_SCLString mhc_FileName;
   static stw_tgl::C_TGLCriticalSection mhc_ConsoleCriticalSection;
   static stw_tgl::C_TGLCriticalSection mhc_FileCriticalSection;
   static std::ofstream mhc_File;

   static void mh_WriteLog(const stw_scl::C_SCLString & orc_Type, const stw_scl::C_SCLString & orc_Activity,
                           const stw_scl::C_SCLString & orc_Message, const stw_types::charn * const opc_Class = NULL,
                           const stw_types::charn * const opc_Function = NULL);
   static void mh_OpenFile(void);
   //Avoid calling
   C_OSCLoggingHandler(void);
   virtual ~C_OSCLoggingHandler(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
