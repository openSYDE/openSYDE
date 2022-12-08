//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Provides functions to use STW flashloader protocol services to interact with flashloader server nodes

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "C_OscFlashProtocolStwFlashloader.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::diag_lib;
using namespace stw::tgl;
using namespace stw::scl;

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
C_OscFlashProtocolStwFlashloader::C_OscFlashProtocolStwFlashloader(const PR_ReportProgress opr_Progress,
                                                                   void * const opv_Instance) :
   C_XFLFlashWrite(),
   pr_ReportProgress(opr_Progress),
   pv_ReportProgressInstance(opv_Instance)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   Tear down class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscFlashProtocolStwFlashloader::~C_OscFlashProtocolStwFlashloader(void)
{
   pr_ReportProgress = NULL;
   pv_ReportProgressInstance = NULL;
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
//lint -e{8001,8011}  //name of function dictated by base class
int32_t C_OscFlashProtocolStwFlashloader::TRG_ReportProgress(const uint16_t ou16_Progress1_1000,
                                                             const C_SclString & orc_AdditionalText)
{
   int32_t s32_Return = C_NO_ERR;

   if (pr_ReportProgress != NULL)
   {
      s32_Return = pr_ReportProgress(pv_ReportProgressInstance, static_cast<uint8_t>(ou16_Progress1_1000 / 10U),
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
//lint -e{8001,8011}  //name of function dictated by base class
void C_OscFlashProtocolStwFlashloader::TRG_ReportStatus(const C_SclString & orc_Text, const uint8_t ou8_Type)
{
   if (pr_ReportProgress != NULL)
   {
      C_SclString c_Text;

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
         break;
      }

      //Signal progress invalid: 255
      pr_ReportProgress(pv_ReportProgressInstance, 0xFFU, c_Text);
   }
}
