//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Utilities

   DiagLib Target Glue Layer module containing utilities
   Implementation for Linux.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <cstring>
#include <unistd.h>
#include <err.h>
#include <pwd.h>
#include <climits>
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report an assertion.

   Show a error message.

   \param[in]     opcn_Module    Module name where the problem turned up
   \param[in]     opcn_Func      Function name where the problem turned up
   \param[in]     os32_Line      Line number where the problem turned up
*/
//----------------------------------------------------------------------------------------------------------------------
void stw::tgl::TglReportAssertion(const char_t * const opcn_Module, const char_t * const opcn_Func,
                                  const int32_t os32_Line)
{
   C_SclString c_Text;

   c_Text = static_cast<C_SclString>("Extremely nasty error\n (assertion in module ") + opcn_Module + ", function " +
            opcn_Func + ", line " + C_SclString::IntToStr(os32_Line) + ") !";
   warnx("%s", c_Text.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report assertion with detail

   Show a messagebox with an detailed error message.

   \param[in]     opcn_DetailInfo        More information
   \param[in]     opcn_Module            Module name where the problem turned up
   \param[in]     opcn_Func              Function name where the problem turned up
   \param[in]     os32_Line              Line number where the problem turned up
*/
//----------------------------------------------------------------------------------------------------------------------
void stw::tgl::TglReportAssertionDetail(const char_t * const opcn_DetailInfo, const char_t * const opcn_Module,
                                        const char_t * const opcn_Func, const int32_t os32_Line)
{
   C_SclString c_Text;

   c_Text = static_cast<C_SclString>(opcn_DetailInfo) +
            "\nfunction  " + opcn_Func +
            "\nassertion in module  " + opcn_Module +
            ", line  " + C_SclString::IntToStr(os32_Line) +
            "!";
   warnx("%s", c_Text.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get system user name

   Reports the name of the currently logged in system user (i.e. the user running the active process)

   Known issue: in the following scenario this function will return false:
      - compiling this with a 32-bit compiler on a 64-bit machine
      - that machine doesn't have the 32-bit (i386) verion of libnss-sss installed
      - user is no local user on the machine but a network user (LDAP)

   \param[out]    orc_UserName     name of logged in user

   \return
   true      user name detected and places in oc_UserName  \n
   false     error -> oc_UserName not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool stw::tgl::TglGetSystemUserName(C_SclString & orc_UserName)
{
   struct passwd * pc_PassWord;
   bool q_Return = false;

   pc_PassWord = getpwuid(geteuid());
   if (pc_PassWord != NULL)
   {
      orc_UserName = pc_PassWord->pw_name;
      q_Return    = true;
   }
   else
   {
      orc_UserName = "\?\?\?\?\?";
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get system machine name

   Reports the name of the system

   \param[out]    orc_MachineName     name of system

   \return
   true      system name detected and placed in orc_MachineName  \n
   false     error -> orc_MachineName not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool stw::tgl::TglGetSystemMachineName(C_SclString & orc_MachineName)
{
   char_t acn_HostName[HOST_NAME_MAX + 1];
   const int x_Result = //lint !e8080 !e970  type defined by API we use
                        gethostname(&acn_HostName[0], sizeof(acn_HostName));

   if (x_Result == 0)
   {
      orc_MachineName = acn_HostName;
   }
   else
   {
      orc_MachineName = "?\?\?\?\?";
   }
   return (x_Result == 0) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Poll system message queue

   Polls the system's message queue and posts detected messages for handling.
   This function can be used to proceed processing system messages while actively waiting for an event.
*/
//----------------------------------------------------------------------------------------------------------------------
void stw::tgl::TglHandleSystemMessages(void)
{
   /*
      MSG t_Msg;
      bool q_Return;
      q_Return = PeekMessage(&t_Msg, 0, 0, 0, PM_REMOVE) == 0 ? false : true;
      if (q_Return == true)
      {
         TranslateMessage(&t_Msg);
         DispatchMessage(&t_Msg);
      }
   */
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set environment variable for calling process

   Sets an environment variable to a fixed value.
   This function was modified to fail in any case after reviewing the code.
   The implementation uses "putenv" which will usually not do what the author expected.
   "putenv" will store a pointer to the passed memory, which is on the stack in this implementation.
   So after the function is left the environment will point to undefined memory.
   See e.g.: https://wiki.sei.cmu.edu/confluence/display/c/POS34-C.+Do+not+call+putenv%28%29+with+a+pointer+to+an+automatic+variable+as+the+argument

   Fixing this is not straightforward. Better for the function to fail than to do random things.
   Maybe Team Linux has a ready-made implementation at hand.

   \param[in]    orc_Name    name of environment variable
   \param[in]    orc_Value   value of environment variable

   \return
   0   variable set
   -1  could not set variable
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t stw::tgl::TglSetEnvironmentVariable(const C_SclString & orc_Name, const C_SclString & orc_Value)
{
   (void)orc_Name;
   (void)orc_Value;
   tgl_assert(false);

#if 0
   int32_t s32_Return = -1;
   char_t acn_String[1024];
   const C_SclString c_String = orc_Name + "=" + orc_Value;

   if (c_String.Length() < sizeof(acn_String))
   {
      std::strcpy(acn_String, c_String.c_str());
      s32_Return = putenv(acn_String);
   }
   return (s32_Return == 0) ? 0 : -1;
#endif
   return -1;
}
