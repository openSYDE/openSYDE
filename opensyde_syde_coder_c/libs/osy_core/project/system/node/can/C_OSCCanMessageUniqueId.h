//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group for message ID part which identifies a unique CAN message
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANMESSAGEUNIQUEID_H
#define C_OSCCANMESSAGEUNIQUEID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanMessageUniqueId
{
public:
   C_OSCCanMessageUniqueId();
   C_OSCCanMessageUniqueId(const stw_types::uint32 ou32_CanId, const bool oq_IsExtended);

   bool operator < (const C_OSCCanMessageUniqueId & orc_Cmp) const;
   bool operator == (const C_OSCCanMessageUniqueId & orc_Cmp) const;
   bool operator != (const C_OSCCanMessageUniqueId & orc_Cmp) const;

   stw_types::uint32 u32_CanId; ///< CAN message identifier
   bool q_IsExtended;           ///< Flag if message id is using extended format
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
