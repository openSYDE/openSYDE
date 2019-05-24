//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI bus: stores additional UI information (header)

   See .cpp file for documentation

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDBUS_H
#define C_PUISDBUS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QPointF>
#include <QColor>
#include <vector>
#include "stwtypes.h"
#include "C_PuiBsLineBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdBus :
   public C_PuiBsLineBase
{
public:
   C_PuiSdBus();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QColor c_UIColorMiddleLine; ///< color of middle line ("spine") of bus
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
