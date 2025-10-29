//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to handle caching of hash values
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVHASHCACHEHANDLING_HPP
#define C_PUISVHASHCACHEHANDLING_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvHashCacheHandling
{
public:
   C_PuiSvHashCacheHandling();

   void SetAllowCash(const bool oq_Value);
   void SetHash(const uint32_t ou32_Value);
   bool GetHashAvailable(void) const;
   uint32_t GetHash(void) const;

private:
   bool mq_AllowCash;
   bool mq_HashIsUpToDate;
   uint32_t mu32_Hash;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
