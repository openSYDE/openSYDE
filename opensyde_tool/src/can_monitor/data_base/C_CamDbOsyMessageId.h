//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID for OSY message
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBOSYMESSAGEID_H
#define C_CAMDBOSYMESSAGEID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbOsyMessageId
{
public:
   C_CamDbOsyMessageId(void);

   stw_types::uint32 u32_Hash;
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices c_Id;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
