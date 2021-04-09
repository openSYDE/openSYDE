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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <pwd.h>
#include "stwtypes.h"
#include "TGLUtils.h"
#include "CSCLString.h"
#include "CSCLResourceStrings.h"
#define STR_TABLE_INCLUDE  //we really want the symbols from the DLStrings.h header
#include "DLStrings.h"

using namespace stw_types;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
static C_SCLResourceStrings mc_ResourceStrings;

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
void stw_tgl::TGL_ReportAssertion(const charn * const opcn_Module, const charn * const opcn_Func,
                                  const sint32 os32_Line)
{
   C_SCLString c_Text;
   c_Text = static_cast<C_SCLString>("Extremely nasty error\n (assertion in module ") + opcn_Module + ", function " +
            opcn_Func + ", line " + C_SCLString::IntToStr(os32_Line) + ") !";
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
void stw_tgl::TGL_ReportAssertionDetail(const charn * const opcn_DetailInfo, const charn * const opcn_Module,
                                        const charn * const opcn_Func, const sint32 os32_Line)
{
   C_SCLString c_Text;
   c_Text = static_cast<C_SCLString>(opcn_DetailInfo)
                                     + "\nfunction  " + opcn_Func
                                     + "\nassertion in module  " + opcn_Module
                                     + ", line  " + C_SCLString::IntToStr(os32_Line)
                                     + "!";
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
bool stw_tgl::TGL_GetSystemUserName(C_SCLString & orc_UserName)
{
   struct passwd *pt_passwd;
   bool q_Return = false;

   pt_passwd = getpwuid(geteuid());
   if (pt_passwd != NULL)
   {
      orc_UserName = pt_passwd->pw_name;
      q_Return    = true;
   }
   else
   {
      orc_UserName = "?????";
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Poll system message queue

   Polls the system's message queue and posts detected messages for handling.
   This function can be used to proceed processing system messages while actively waiting for an event.
*/
//----------------------------------------------------------------------------------------------------------------------
void stw_tgl::TGL_HandleSystemMessages(void)
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
/*! \brief  Load resource string

   Load resource strings from resource table.
   For Windows this is easy as calling "LoadString".
   For other target we will have to use the C_SCLResourceStrings class
    and fill a singleton of it with the application strings (e.g. VisLibString.h).
   So we probably will need an init function as well.

   \param[in]    ou16_StringIndex     Index of string

   \return
   string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString stw_tgl::TGL_LoadStr(const uint16 ou16_StringIndex)
{
   static bool hq_Initialized = false;

   if (hq_Initialized == false)
   {
      mc_ResourceStrings.SetStringTable(gac_DIAG_LIB_RESOURCE_STRINGS, gu16_DIAGLIB_NR_RES_STRNGS);
      hq_Initialized = true;
   }

   return mc_ResourceStrings.LoadStr(ou16_StringIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set environment variable for calling process

   Sets an environment variable to a fixed value.

   \param[in]    orc_Name    name of environment variable
   \param[in]    orc_Value   value of environment variable

   \return
   0   variable set
   -1  could not set variable
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 TGL_PACKAGE stw_tgl::TGL_SetEnvironmentVariable(const C_SCLString & orc_Name, const C_SCLString & orc_Value)
{
   sintn sn_Return = -1;
   char acn_String[1024];
   C_SCLString c_String = orc_Name + "=" + orc_Value;

   if (c_String.Length() < sizeof(acn_String))
   {
      strcpy(acn_String, c_String.c_str());
      sn_Return = putenv(acn_String);
   }
   return (sn_Return == 0) ? 0 : -1;
}
