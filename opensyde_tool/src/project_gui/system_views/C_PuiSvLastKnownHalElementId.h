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
#include "stwtypes.h"

#include <QString>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvLastKnownHalElementId
{
public:
   C_PuiSvLastKnownHalElementId();
   C_PuiSvLastKnownHalElementId(const stw_types::uint32 ou32_Crc, const QString & orc_HalDpName);
   virtual ~C_PuiSvLastKnownHalElementId();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_types::uint32 u32_Crc;
   QString c_HalDpName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
