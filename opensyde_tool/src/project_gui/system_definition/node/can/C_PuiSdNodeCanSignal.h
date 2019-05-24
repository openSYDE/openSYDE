//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a signal positioning in a CAN message UI part (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODECANSIGNAL_H
#define C_PUISDNODECANSIGNAL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeCanSignal
{
public:
   C_PuiSdNodeCanSignal(void);

   stw_types::uint8 u8_ColorIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
