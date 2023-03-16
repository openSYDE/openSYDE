//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID to allow reidentification of HAL elements
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVLASTKNOWNHALELEMENTID_H
#define C_PUISVLASTKNOWNHALELEMENTID_H

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

class C_PuiSvLastKnownHalElementId
{
public:
   C_PuiSvLastKnownHalElementId();
   C_PuiSvLastKnownHalElementId(const uint32_t ou32_Crc, const QString & orc_HalDpName);
   virtual ~C_PuiSvLastKnownHalElementId();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   uint32_t u32_Crc;
   QString c_HalDpName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
