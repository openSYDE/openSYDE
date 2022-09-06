//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen mapping entry for a signal
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENMANAGERMAPPABLESIGNAL_H
#define C_OSCCANOPENMANAGERMAPPABLESIGNAL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCCanSignal.h"
#include "C_OSCNodeDataPoolListElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanOpenManagerMappableSignal
{
public:
   C_OSCCanOpenManagerMappableSignal();

   bool q_AutoMinMaxUsed;
   C_OSCCanSignal c_SignalData;
   C_OSCNodeDataPoolListElement c_DatapoolData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
