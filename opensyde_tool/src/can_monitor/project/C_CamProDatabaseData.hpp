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

#include "stwtypes.hpp"
#include "C_OscComMessageLogger.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_CamProDatabaseData
{
public:
   C_CamProDatabaseData(void);

   QString c_Name;
   bool q_Enabled;
   int32_t s32_BusIndex;

   void CalcHash(uint32_t & oru32_HashValue) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
