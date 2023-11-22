//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group for message ID part which identifies a unique CAN message
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANMESSAGEUNIQUEID_HPP
#define C_OSCCANMESSAGEUNIQUEID_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanMessageUniqueId
{
public:
   C_OscCanMessageUniqueId();
   C_OscCanMessageUniqueId(const uint32_t ou32_CanId, const bool oq_IsExtended);

   bool operator < (const C_OscCanMessageUniqueId & orc_Cmp) const;
   bool operator == (const C_OscCanMessageUniqueId & orc_Cmp) const;
   bool operator != (const C_OscCanMessageUniqueId & orc_Cmp) const;

   uint32_t u32_CanId; ///< CAN message identifier
   bool q_IsExtended;  ///< Flag if message id is using extended format
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
