//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard configure button (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBCONFIGURE_H
#define C_OGEPUBCONFIGURE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubConfigure :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubConfigure(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
