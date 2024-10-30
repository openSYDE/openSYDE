//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID to allow reidentification of HAL elements
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDLASTKNOWNHALELEMENTID_HPP
#define C_PUISDLASTKNOWNHALELEMENTID_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include <QString>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdLastKnownHalElementId
{
public:
   C_PuiSdLastKnownHalElementId();
   C_PuiSdLastKnownHalElementId(const uint32_t ou32_Crc, const QString & orc_HalDpName);
   virtual ~C_PuiSdLastKnownHalElementId();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   uint32_t u32_Crc;
   QString c_HalDpName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
