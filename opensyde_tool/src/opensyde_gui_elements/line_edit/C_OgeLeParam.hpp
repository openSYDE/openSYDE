//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom line edit for param widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEPARAM_H
#define C_OGELEPARAM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeParam :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeLeParam(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
