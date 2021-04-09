//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Utilities

   DiagLib Target Glue Layer module containing utilities

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef TGLUTILSH
#define TGLUTILSH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"

#ifndef TGL_PACKAGE
#ifdef __BORLANDC__
#define TGL_PACKAGE __declspec(package)
#else
#define TGL_PACKAGE
#endif
#endif

#if __cplusplus >= 201103L //C++11 ?
#define TGL_UTIL_FUNC_ID __func__
#else
#ifdef __BORLANDC__
#define TGL_UTIL_FUNC_ID __FUNC__
#else
#define TGL_UTIL_FUNC_ID __FUNCTION__
#endif
#endif

//PC-Lint suppressions: function-like macro is the only way to get file,func,line information into the text
//lint -save -e1960
#ifndef tgl_assert
#define tgl_assert(p) ((p) ? (void)0 : stw_tgl::TGL_ReportAssertion(__FILE__, TGL_UTIL_FUNC_ID, __LINE__))
#define tgl_assertdetail(a, b) ((a) ? (void)0 : stw_tgl::TGL_ReportAssertionDetail((b), __FILE__, TGL_UTIL_FUNC_ID, \
                                __LINE__))
#endif
//lint -restore

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_tgl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

void TGL_PACKAGE TGL_ReportAssertion(const stw_types::charn * const opcn_Module,
                                     const stw_types::charn * const opcn_Func, const stw_types::sint32 os32_Line);
void TGL_PACKAGE TGL_ReportAssertionDetail(const stw_types::charn * const opcn_DetailInfo,
                                           const stw_types::charn * const opcn_Module,
                                           const stw_types::charn * const opcn_Func, const stw_types::sint32 os32_Line);
bool TGL_PACKAGE TGL_GetSystemUserName(stw_scl::C_SCLString & orc_UserName);
void TGL_PACKAGE TGL_HandleSystemMessages(void);

stw_types::sint32 TGL_PACKAGE TGL_SetEnvironmentVariable(const stw_scl::C_SCLString & orc_Name,
                                                         const stw_scl::C_SCLString & orc_Value);

//resource string handling:
stw_scl::C_SCLString TGL_PACKAGE TGL_LoadStr(const stw_types::uint16 ou16_StringIndex);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
