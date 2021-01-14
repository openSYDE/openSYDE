//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID for unmapped DBC message
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBDBCUNMAPPEDMESSAGEID_H
#define C_CAMDBDBCUNMAPPEDMESSAGEID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbDbcUnmappedMessageId
{
public:
   C_CamDbDbcUnmappedMessageId(void);

   stw_types::uint32 u32_Hash;
   stw_types::uint32 u32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
