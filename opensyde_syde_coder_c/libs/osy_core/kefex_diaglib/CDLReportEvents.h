//-----------------------------------------------------------------------------
/*!
   \file        CDLReportEvents.h
   \brief       Interface class for reporting events during processes

   Provides an interface to reporting progress and status during potentially longer function calls and processes.

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.06.2010  STW/A.Stangl
   \endimplementation

   \internal
   \history
   Date(dd.mm.yyyy)  Author        Description
   22.06.2010        STW/A.Stangl  file created
   \endhistory
*/
//-----------------------------------------------------------------------------
#ifndef CDLREPORTEVENTSH
#define CDLREPORTEVENTSH

/* -- Includes ------------------------------------------------------------ */
#include "stwtypes.h"
#include "DiagLib_config.h"
#include "CSCLString.h"

namespace stw_diag_lib
{

/* -- Defines ------------------------------------------------------------- */
const stw_types::uint8 gu8_DL_REPORT_STATUS_TYPE_INFORMATION = 0U; ///< just some general progress information
const stw_types::uint8 gu8_DL_REPORT_STATUS_TYPE_WARNING     = 1U; ///< warning -> will not result in the process aborting
const stw_types::uint8 gu8_DL_REPORT_STATUS_TYPE_ERROR       = 2U; ///< error -> will result in the process aborting

/* -- Types --------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------- */
///Abstract base class providing an interface for implementation of reported functions
class STW_DIAGLIB_EXPORT C_DLReportEvents
{
public:
protected:
   C_DLReportEvents(void);
   virtual ~C_DLReportEvents(void);

   //Report progress (during a lengthy process)
   //This is just for informational purposes and not neccessarily exactly linear to the actual progress.
   //The return value can be used to abort an ongoing procdure (if supported by the caller).
   virtual stw_types::sint32 TRG_ReportProgress(const stw_types::uint16 ou16_Progress1_1000,
                                                const stw_scl::C_SCLString & orc_AdditionalText) = 0;
   //report error or warning text (e.g. log or messagebox)
   //for "type" cf. "gu8_DL_REPORT_STATUS_TYPE_..." constants
   virtual void TRG_ReportStatus(const stw_scl::C_SCLString & orc_Text, const stw_types::uint8 ou8_Type) = 0;
};

}

/* -- Implementation ------------------------------------------------------ */

#endif
