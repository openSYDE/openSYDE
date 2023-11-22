//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID for unmapped DBC message
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBDBCUNMAPPEDMESSAGEID_HPP
#define C_CAMDBDBCUNMAPPEDMESSAGEID_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbDbcUnmappedMessageId
{
public:
   C_CamDbDbcUnmappedMessageId(void);

   uint32_t u32_Hash;
   uint32_t u32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
