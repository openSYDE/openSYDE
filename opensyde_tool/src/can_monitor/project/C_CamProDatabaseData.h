//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Database data structure (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMPRODATABASEDATA_H
#define C_CAMPRODATABASEDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QList>

#include "stwtypes.h"
#include "C_OSCComMessageLogger.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_CamProDatabaseData
{
public:
   C_CamProDatabaseData(void);

   QString c_Name;
   bool q_Enabled;
   stw_types::sint32 s32_BusIndex;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
