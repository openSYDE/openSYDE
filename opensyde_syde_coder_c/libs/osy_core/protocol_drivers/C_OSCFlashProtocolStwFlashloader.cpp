//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Provides functions to use STW flashloader protocol services to interact with flashloader server nodes

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "TGLUtils.h"
#include "C_OSCFlashProtocolStwFlashloader.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_diag_lib;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Set up class

   \param[in]  opr_Progress  function to call when reporting progress
   \param[in]  opv_Instance  instance pointer to pass as parameter when reporting progress
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCFlashProtocolStwFlashloader::C_OSCFlashProtocolStwFlashloader(const PR_ReportProgress opr_Progress,
                                                                   void * const opv_Instance) :
   C_XFLFlashWrite(),
   mpr_ReportProgress(opr_Progress),
   mpv_ReportProgressInstance(opv_Instance)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   Tear down class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCFlashProtocolStwFlashloader::~C_OSCFlashProtocolStwFlashloader(void)
{
   mpr_ReportProgress = NULL;
   mpv_ReportProgressInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   report progress to application

   Convert progress information from STW Flashloader driver to openSYDE format and invoke installed reporting function

   \param[in]   ou16_Progress1_1000    progress of operation (from 0 to 1000)
   \param[in]   orc_AdditionalText     textual information about progress

   \return
   C_NO_ERR    continue operation
   else        abort operation (not honored at each position)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCFlashProtocolStwFlashloader::TRG_ReportProgress(const uint16 ou16_Progress1_1000,
                                                            const C_SCLString & orc_AdditionalText)
{
   sint32 s32_Return = C_NO_ERR;

   if (mpr_ReportProgress != NULL)
   {
      s32_Return = mpr_ReportProgress(mpv_ReportProgressInstance, static_cast<uint8>(ou16_Progress1_1000 / 10U),
                                      "Information: " + orc_AdditionalText);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   report status to application

   Convert status information from STW Flashloader driver to openSYDE format and invoke installed reporting function

   \param[in]   orc_Text          textual information about status
   \param[in]   ou8_Type          type of reported status (see gu8_DL_REPORT_STATUS_TYPE_* constants)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCFlashProtocolStwFlashloader::TRG_ReportStatus(const C_SCLString & orc_Text, const uint8 ou8_Type)
{
   if (mpr_ReportProgress != NULL)
   {
      C_SCLString c_Text;

      switch (ou8_Type)
      {
      case gu8_DL_REPORT_STATUS_TYPE_ERROR:
         osc_write_log_error("STW Flashloader driver", orc_Text);
         c_Text = "Error: " + orc_Text;
         break;
      case gu8_DL_REPORT_STATUS_TYPE_INFORMATION:
         c_Text = "Information: " + orc_Text;
         break;
      case gu8_DL_REPORT_STATUS_TYPE_WARNING:
         osc_write_log_warning("STW Flashloader driver", orc_Text);
         c_Text = "Warning: " + orc_Text;
         break;
      default:
         tgl_assert(false);
         break;
      }

      //Signal progress invalid: 255
      mpr_ReportProgress(mpv_ReportProgressInstance, 0xFFU, c_Text);
   }
}
