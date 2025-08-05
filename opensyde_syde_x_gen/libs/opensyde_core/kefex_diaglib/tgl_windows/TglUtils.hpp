//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Utilities

   DiagLib Target Glue Layer module containing utilities

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef TGLUTILSHPP
#define TGLUTILSHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

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
//lint -save -e9026
#ifndef tgl_assert
#define tgl_assert(p) ((p) ? (void)0 : stw::tgl::TglReportAssertion(__FILE__, TGL_UTIL_FUNC_ID, __LINE__))
#define tgl_assertdetail(a, b) ((a) ? (void)0 : stw::tgl::TglReportAssertionDetail((b), __FILE__, TGL_UTIL_FUNC_ID, \
                                                                                   __LINE__))
#endif
//lint -restore

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace tgl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

void TglReportAssertion(const char_t * const opcn_Module, const char_t * const opcn_Func, const int32_t os32_Line);
void TglReportAssertionDetail(const char_t * const opcn_DetailInfo, const char_t * const opcn_Module,
                              const char_t * const opcn_Func, const int32_t os32_Line);
bool TglGetSystemUserName(stw::scl::C_SclString & orc_UserName);
bool TglGetSystemMachineName(stw::scl::C_SclString & orc_MachineName);
void TglHandleSystemMessages(void);

int32_t TglSetEnvironmentVariable(const stw::scl::C_SclString & orc_Name, const stw::scl::C_SclString & orc_Value);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
