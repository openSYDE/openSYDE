//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID for OSY list
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBOSYLISTID_HPP
#define C_CAMDBOSYLISTID_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNodeDataPoolListId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbOsyListId
{
public:
   C_CamDbOsyListId(void);

   uint32_t u32_Hash;
   stw::opensyde_core::C_OscNodeDataPoolListId c_Id;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
