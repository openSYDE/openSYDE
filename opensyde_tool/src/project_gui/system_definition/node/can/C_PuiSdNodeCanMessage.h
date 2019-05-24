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
#include "C_PuiSdNodeCanSignal.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeCanMessage
{
public:
   C_PuiSdNodeCanMessage(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   bool q_UseAutoReceiveTimeout;
   std::vector<C_PuiSdNodeCanSignal> c_Signals;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
