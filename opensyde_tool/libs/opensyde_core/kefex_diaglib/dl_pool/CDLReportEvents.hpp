//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interface class for reporting events during processes

   Provides an interface to reporting progress and status during potentially longer function calls and processes.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CDLREPORTEVENTSHPP
#define CDLREPORTEVENTSHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "DiagLib_config.hpp"
#include "C_SclString.hpp"

namespace stw
{
namespace diag_lib
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */
const uint8_t gu8_DL_REPORT_STATUS_TYPE_INFORMATION = 0U; ///< just some general progress information
const uint8_t gu8_DL_REPORT_STATUS_TYPE_WARNING     = 1U; ///< warning -> will not result in the process aborting
const uint8_t gu8_DL_REPORT_STATUS_TYPE_ERROR       = 2U; ///< error -> will result in the process aborting

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */
///Abstract base class providing an interface for implementation of reported functions
class C_DLReportEvents
{
public:
protected:
   C_DLReportEvents(void);
   virtual ~C_DLReportEvents(void);

   //Report progress (during a lengthy process)
   //This is just for informational purposes and not necessarily exactly linear to the actual progress.
   //The return value can be used to abort an ongoing procedure (if supported by the caller).
   virtual int32_t TRG_ReportProgress(const uint16_t ou16_Progress1_1000,
                                      const stw::scl::C_SclString & orc_AdditionalText) = 0;
   //report error or warning text (e.g. log or messagebox)
   //for "type" cf. "gu8_DL_REPORT_STATUS_TYPE_..." constants
   virtual void TRG_ReportStatus(const stw::scl::C_SclString & orc_Text, const uint8_t ou8_Type) = 0;
};
}
}
/* -- Implementation ------------------------------------------------------------------------------------------------ */

#endif
