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
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbOsyMessageId
{
public:
   C_CamDbOsyMessageId(void);

   uint32_t u32_Hash;
   stw::opensyde_core::C_OscCanMessageIdentificationIndices c_Id;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
