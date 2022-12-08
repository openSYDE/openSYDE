//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CAN communication message related, additional information UI part (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODECANMESSAGE_H
#define C_PUISDNODECANMESSAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_PuiSdNodeCanSignal.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeCanMessage
{
public:
   C_PuiSdNodeCanMessage(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   enum E_RxTimeoutMode ///< Rx timeout mode
   {
      eRX_TIMEOUT_MODE_CUSTOM,  ///< Timeout time is defined by user
      eRX_TIMEOUT_MODE_AUTO,    ///< Timeout time is calculated automatically
      eRX_TIMEOUT_MODE_DISABLED ///< Timeout monitoring is disabled for the message (only for on event)
   };

   E_RxTimeoutMode e_ReceiveTimeoutMode;
   std::vector<C_PuiSdNodeCanSignal> c_Signals;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
