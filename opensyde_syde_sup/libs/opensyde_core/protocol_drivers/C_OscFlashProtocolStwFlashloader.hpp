//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Provides functions to use STW flashloader protocol services to interact with flashloader server nodes

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCFLASHPROTOCOLSTWFLASHLOADER_H
#define C_OSCFLASHPROTOCOLSTWFLASHLOADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CXFLFlashWrite.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscFlashProtocolStwFlashloader :
   public stw::diag_lib::C_XFLFlashWrite
{
public:
   typedef int32_t (* PR_ReportProgress)(void * const opv_Instance, const uint8_t ou8_Progress,
                                         const stw::scl::C_SclString & orc_Text);

   PR_ReportProgress pr_ReportProgress;
   void * pv_ReportProgressInstance;

   C_OscFlashProtocolStwFlashloader(const PR_ReportProgress opr_Progress, void * const opv_Instance);
   virtual ~C_OscFlashProtocolStwFlashloader(void);

protected:
   //reporting functions from STW Flashloader protocol driver
   ///Report progress in 1/1000; return a value != C_NO_ERR to signal the flashing procedure to abort.
   ///This is just for informational purposes and not necessarily exactly linear to the actual progress.
   //lint -e{8001,8011}  //name of function dictated by base class
   virtual int32_t TRG_ReportProgress(const uint16_t ou16_Progress1_1000,
                                      const stw::scl::C_SclString & orc_AdditionalText);
   ///report status as text; can be used for logging what's going on
   //lint -e{8001,8011}  //name of function dictated by base class
   virtual void TRG_ReportStatus(const stw::scl::C_SclString & orc_Text, const uint8_t ou8_Type);

private:
   //Avoid call
   C_OscFlashProtocolStwFlashloader(const C_OscFlashProtocolStwFlashloader &);
   C_OscFlashProtocolStwFlashloader & operator =(const C_OscFlashProtocolStwFlashloader &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
