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
#include "stwtypes.hpp"
#include "C_PuiBsLineBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdBus :
   public C_PuiBsLineBase
{
public:
   C_PuiSdBus();

   void CalcHash(uint32_t & oru32_HashValue) const override;

   QColor c_UiColorMiddleLine; ///< color of middle line ("spine") of bus
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
