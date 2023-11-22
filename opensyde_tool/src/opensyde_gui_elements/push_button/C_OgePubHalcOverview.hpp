//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Overview button in HALC Screen.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBHALCOVERVIEW_HPP
#define C_OGEPUBHALCOVERVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubHalcOverview :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubHalcOverview(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
