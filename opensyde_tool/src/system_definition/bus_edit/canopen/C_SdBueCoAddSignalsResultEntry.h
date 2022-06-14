//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen result entry for add signals tree
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOADDSIGNALSRESULTENTRY_H
#define C_SDBUECOADDSIGNALSRESULTENTRY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCCanSignal.h"
#include "C_OSCNodeDataPoolListElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueCoAddSignalsResultEntry
{
public:
   C_SdBueCoAddSignalsResultEntry();

   bool q_AutoMinMaxUsed;
   stw_opensyde_core::C_OSCCanSignal c_SignalData;
   stw_opensyde_core::C_OSCNodeDataPoolListElement c_DatapoolData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
