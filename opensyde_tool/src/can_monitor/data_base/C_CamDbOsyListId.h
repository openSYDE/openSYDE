//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID for OSY list
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBOSYLISTID_H
#define C_CAMDBOSYLISTID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCNodeDataPoolListId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbOsyListId
{
public:
   C_CamDbOsyListId(void);

   stw_types::uint32 u32_Hash;
   stw_opensyde_core::C_OSCNodeDataPoolListId c_Id;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
