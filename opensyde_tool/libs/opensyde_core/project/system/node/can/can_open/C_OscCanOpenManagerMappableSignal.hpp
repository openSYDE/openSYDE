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
#include "C_OscCanSignal.hpp"
#include "C_OscNodeDataPoolListElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanOpenManagerMappableSignal
{
public:
   C_OscCanOpenManagerMappableSignal();

   bool q_AutoMinMaxUsed;
   C_OscCanSignal c_SignalData;
   C_OscNodeDataPoolListElement c_DatapoolData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
