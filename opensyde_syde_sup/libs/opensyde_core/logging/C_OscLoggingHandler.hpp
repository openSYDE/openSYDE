//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for logging operations (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCLOGGINGHANDLER_H
#define C_OSCLOGGINGHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <fstream>
#include <map>
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "TglTasks.hpp"

//PC-Lint suppressions: function-like macro is the only way to get file,func,line information into the text
//lint -save -e9026
#define osc_write_log_info(activity, message) \
   (stw::opensyde_core::C_OscLoggingHandler::h_WriteLogInfo((activity), (message), \
                                                            __FILE__, TGL_UTIL_FUNC_ID))
#define osc_write_log_warning(activity, message) \
   (stw::opensyde_core::C_OscLoggingHandler::h_WriteLogWarning((activity), (message), \
                                                               __FILE__, TGL_UTIL_FUNC_ID))
#define osc_write_log_error(activity, message) \
   (stw::opensyde_core::C_OscLoggingHandler::h_WriteLogError((activity), (message), \
                                                             __FILE__, TGL_UTIL_FUNC_ID))
#define osc_write_log_performance_start() (stw::opensyde_core::C_OscLoggingHandler::h_StartPerformanceTimer())
#define osc_write_log_performance_stop(timerid, message) \
   (stw::opensyde_core::C_OscLoggingHandler::h_WriteLogPerformance((timerid), (message), __FILE__, TGL_UTIL_FUNC_ID))
//lint -restore

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Handler class for logging operations
class C_OscLoggingHandler
{
public:
   //Configuration
   static void h_SetWriteToFileActive(const bool oq_Active);
   static void h_SetWriteToConsoleActive(const bool oq_Active);
   static void h_SetMeasurePerformanceActive(const bool oq_Active);
   static void h_SetCompleteLogFileLocation(const stw::scl::C_SclString & orc_CompleteLogFileLocation);
   static const stw::scl::C_SclString & h_GetCompleteLogFileLocation(void);

   //Logging call
   static void h_WriteLogInfo(const stw::scl::C_SclString & orc_Activity, const stw::scl::C_SclString & orc_Message,
                              const char_t * const opcn_Class = NULL, const char_t * const opcn_Function = NULL);
   static void h_WriteLogWarning(const stw::scl::C_SclString & orc_Activity, const stw::scl::C_SclString & orc_Message,
                                 const char_t * const opcn_Class = NULL, const char_t * const opcn_Function = NULL);
   static void h_WriteLogError(const stw::scl::C_SclString & orc_Activity, const stw::scl::C_SclString & orc_Message,
                               const char_t * const opcn_Class = NULL, const char_t * const opcn_Function = NULL);
   static void h_WriteLogPerformance(const uint16_t ou16_TimerId, const stw::scl::C_SclString & orc_Message,
                                     const char_t * const opcn_Class = NULL, const char_t * const opcn_Function = NULL);

   static uint16_t h_StartPerformanceTimer(void);
   static stw::scl::C_SclString h_StwError(const int32_t os32_Error);

   //Utility functions
   static void h_Flush(void);
   static std::string h_UtilConvertDateTimeToString(const stw::tgl::C_TglDateTime & orc_DateTime);

private:
   static bool mhq_WriteToFile;
   static bool mhq_WriteToConsole;
   static bool mhq_MeasureTime;
   static std::map<uint16_t, uint32_t> mhc_StartTimes; ///< first: Timer ID, second: start time
   static stw::scl::C_SclString mhc_FileName;
   static stw::tgl::C_TglCriticalSection mhc_ConsoleCriticalSection;
   static stw::tgl::C_TglCriticalSection mhc_FileCriticalSection;
   static std::ofstream mhc_File;

   static void mh_WriteLog(const stw::scl::C_SclString & orc_Type, const stw::scl::C_SclString & orc_Activity,
                           const stw::scl::C_SclString & orc_Message, const char_t * const opcn_Class = NULL,
                           const char_t * const opcn_Function = NULL);
   static void mh_OpenFile(void);
   //Avoid calling
   C_OscLoggingHandler(void);
   virtual ~C_OscLoggingHandler(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
